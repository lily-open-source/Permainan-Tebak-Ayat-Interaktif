#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DFRobotDFPlayerMini.h>
#include "track.h"  // Pastikan file ini ada dan berisi definisi nama track

DFRobotDFPlayerMini dfPlayer;
LiquidCrystal_I2C lcd(0x22, 20, 4);

// Konfigurasi Keypad
const byte ROWS = 3;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'Y', 'N', '0', '1'},
  {'2', '3', '4', '5'},
  {'6', '7', '8', '9'}
};
byte rowPins[ROWS] = {5, 4, 3};
byte colPins[COLS] = {9, 8, 7, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Konfigurasi pin
const int ledPin = 50;
const int buzzerPin = 51;

// Variabel
int skor = 5;
int nomorFile;
unsigned long waktuMulaiBuzzer = 0;
const unsigned long durasiCuitan = 500;
bool audioSedangDiputar = false;
unsigned long waktuPesanMulai = 0;
const unsigned long durasiPesan = 2000;
bool pesanAktif = false;
bool sdCardStatus = false;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Periksa komponen
  bool allComponentsOK = periksaKomponen();

  if (allComponentsOK) {
    sistemBerjalan();
    tampilkanMenuUtama();
  } else {
    lcd.clear();
    lcd.print("Pemeriksaan gagal");
    lcd.setCursor(0, 1);
    lcd.print("Periksa perangkat");
    while (true);  // Hentikan program
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Handle buzzer timing
  if (waktuMulaiBuzzer > 0 && currentMillis - waktuMulaiBuzzer >= durasiCuitan) {
    digitalWrite(buzzerPin, LOW);
    waktuMulaiBuzzer = 0;
  }

  // Check audio playback status
  if (audioSedangDiputar && dfPlayer.available() && dfPlayer.readType() == DFPlayerPlayFinished) {
    audioSedangDiputar = false;
    tampilkanMenuUtama();
  }

  // Check keypad input
  char key = keypad.getKey();
  if (key) {
    prosesInputKeypad(key);
  }

  // Handle message display duration
  if (pesanAktif && currentMillis - waktuPesanMulai >= durasiPesan) {
    pesanAktif = false;
    tampilkanMenuUtama();
  }

  // Cek event dari DFPlayer
  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }
}

bool periksaKomponen() {
  bool status = true;

  // Periksa DFPlayer dan kartu SD
  lcd.clear();
  lcd.print("Memeriksa DFPlayer");
  lcd.setCursor(0, 1);
  lcd.print("dan kartu SD...");
  
  if (!dfPlayer.begin(Serial1)) {
    lcd.clear();
    lcd.print("DFPlayer gagal!");
    lcd.setCursor(0, 1);
    lcd.print("Periksa koneksi");
    status = false;
  } else {
    // Coba baca jumlah file di kartu SD
    int jumlahFile = dfPlayer.readFileCounts();
    if (jumlahFile == -1) {
      lcd.clear();
      lcd.print("Kartu SD gagal!");
      lcd.setCursor(0, 1);
      lcd.print("Periksa kartu SD");
      status = false;
    } else {
      lcd.clear();
      lcd.print("DFPlayer & SD OK");
      lcd.setCursor(0, 1);
      lcd.print("File: ");
      lcd.print(jumlahFile);
      sdCardStatus = true;
    }
  }
  delay(2000);

  // Periksa LCD (jika sampai di sini, LCD bekerja)
  lcd.clear();
  lcd.print("Memeriksa LCD...");
  lcd.setCursor(0, 1);
  lcd.print("LCD OK");
  delay(1000);

  // Periksa LED
  lcd.clear();
  lcd.print("Memeriksa LED...");
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  lcd.setCursor(0, 1);
  lcd.print("LED OK");
  delay(1000);

  // Periksa Buzzer
  lcd.clear();
  lcd.print("Memeriksa Buzzer...");
  tone(buzzerPin, 1000, 500);
  lcd.setCursor(0, 1);
  lcd.print("Buzzer OK");
  delay(1000);

  return status;
}

void sistemBerjalan() {
  digitalWrite(ledPin, HIGH);
}

void tampilkanMenuUtama() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selamat Datang!");
  lcd.setCursor(0, 1);
  lcd.print("Pilih Permainan:");
  lcd.setCursor(0, 2);
  lcd.print("1: Dengar Ayat");
  lcd.setCursor(0, 3);
  lcd.print("2: Tebak Ayat");
}

void prosesInputKeypad(char key) {
  if (!audioSedangDiputar && !pesanAktif) {
    if (key == '1') {
      mintaKonfirmasi("Yakin mendengar?", saatYaMendengarkan, saatTidakMendengarkan);
    } else if (key == '2') {
      mintaKonfirmasi("Yakin menebak?", saatYaMenebak, saatTidakMenebak);
    }
  }
}

void mintaKonfirmasi(const char* pesan, void (*callbackYa)(), void (*callbackTidak)()) {
  lcd.clear();
  lcd.print(pesan);
  lcd.setCursor(0, 1);
  lcd.print("Y: Ya  N: Tidak");

  while (true) {
    char key = keypad.getKey();
    if (key == 'Y') {
      callbackYa();
      break;
    }
    if (key == 'N') {
      callbackTidak();
      break;
    }
  }
}

void modeMendengarkan() {
  lcd.clear();
  lcd.print("Masukkan nomor:");
  char input[4] = "";
  dapatkanInputKeypad(input, sizeof(input));
  nomorFile = atoi(input);

  mintaKonfirmasi("Putar file ini?", saatPutarFile, tampilkanMenuUtama);
}

void modeMenebak() {
  nomorFile = random(1, 287);
  playAudioFile(nomorFile);
  audioSedangDiputar = true;

  lcd.setCursor(15, 0);
  lcd.print("Skor:");
  lcd.print(skor);

  mintaKonfirmasi("Tebak nomor?", saatTebakanJawaban, tampilkanMenuUtama);
}

void dapatkanInputKeypad(char* bufferInput, size_t ukuranBuffer) {
  size_t indeks = 0;
  while (indeks < ukuranBuffer - 1) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        bufferInput[indeks++] = key;
        lcd.setCursor(indeks, 1);
        lcd.print(key);
      }
    }
  }
  bufferInput[indeks] = '\0';
}

void playAudioFile(int trackNumber) {
  dfPlayer.play(trackNumber);
  audioSedangDiputar = true;
  lcd.clear();
  lcd.print("Memutar file ");
  lcd.setCursor(0, 1);
  lcd.print(trackNames[trackNumber - 1]);  // Tampilkan nama track
}

void saatYaMendengarkan() {
  modeMendengarkan();
}

void saatTidakMendengarkan() {
  tampilkanMenuUtama();
}

void saatYaMenebak() {
  modeMenebak();
}

void saatTidakMenebak() {
  tampilkanMenuUtama();
}

void saatPutarFile() {
  playAudioFile(nomorFile);
}

void saatTebakanJawaban() {
  lcd.clear();
  lcd.print("Masukkan tebakan:");
  char input[4] = "";
  dapatkanInputKeypad(input, sizeof(input));
  int tebakan = atoi(input);

  if (tebakan == nomorFile) {
    skor++;
    dfPlayer.play(287); // Putar suara "jawaban benar"
    tampilkanPesan(("Benar! Skor: " + String(skor)).c_str());
  } else {
    skor--;
    dfPlayer.play(286); // Putar suara "jawaban salah"
    jawabanSalah();
    tampilkanPesan(("Salah! Skor: " + String(skor)).c_str());
  }

  if (skor == 0) {
    tampilkanPesan("Permainan Selesai");
    dfPlayer.play(288); // Putar suara "game over"
  } else if (skor == 10) {
    tampilkanPesan("Anda Menang!");
    dfPlayer.play(289); // Putar suara "menang"
  } else {
    mintaKonfirmasi("Main lagi?", saatYaMendengarkan, tampilkanMenuUtama);
  }
}

void jawabanSalah() {
  digitalWrite(buzzerPin, HIGH);
  waktuMulaiBuzzer = millis();
}

void tampilkanPesan(const char* pesan) {
  lcd.clear();
  lcd.print(pesan);
  waktuPesanMulai = millis();
  pesanAktif = true;
}

void prosesKartuSDEvent(uint8_t event) {
  switch(event) {
    case DFPlayerCardInserted:
      sdCardStatus = true;
      lcd.clear();
      lcd.print("Kartu SD dimasukkan");
      break;
    case DFPlayerCardRemoved:
      sdCardStatus = false;
      lcd.clear();
      lcd.print("Kartu SD dicabut");
      break;
  }
  delay(2000);
  tampilkanMenuUtama();
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      prosesKartuSDEvent(DFPlayerCardInserted);
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      prosesKartuSDEvent(DFPlayerCardRemoved);
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
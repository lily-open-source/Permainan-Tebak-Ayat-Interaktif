// Uncomment the following line to enable extended listening mode
//#define EXTENDED_LISTENING_MODE

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SD.h>
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>
#include "track.h"  // Include the track names

DFRobotDFPlayerMini dfPlayer;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Keypad configuration
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

// Pin configuration
const int ledPin = 50;
const int buzzerPin = 51;

// Variables
int skor = 5;
int nomorFile;
unsigned long waktuMulaiBuzzer = 0;
const unsigned long durasiCuitan = 500;
bool audioSedangDiputar = false;
unsigned long waktuPesanMulai = 0;
const unsigned long durasiPesan = 2000;
bool pesanAktif = false;

// Function declarations
void tampilkanMenuUtama();
void modeMendengarkan();
void modeMenebak();
void playAudioFile(int trackNumber);
void mintaKonfirmasi(const char* pesan, void (*callbackYa)(), void (*callbackTidak)());
void dapatkanInputKeypad(char* bufferInput, size_t ukuranBuffer);
void prosesInputKeypad(char key);
void sistemBerjalan();
void jawabanSalah();
void saatYaMendengarkan();
void saatTidakMendengarkan();
void saatYaMenebak();
void saatTidakMenebak();
void saatPutarFile();
void saatTebakanJawaban();
void pilihCaraMendengarkan();
void mendengarkanManual();
void mendengarkanRandom();
void tampilkanPesan(const char* pesan);

// Setup function
void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  if (!dfPlayer.begin(Serial1)) {
    Serial.println("DFPlayer Mini tidak terdeteksi.");
    while (true);
  }

  sistemBerjalan();
  tampilkanMenuUtama();
}

// Main loop
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
}

// Process keypad input
void prosesInputKeypad(char key) {
  if (!audioSedangDiputar && !pesanAktif) {
    if (key == '1') {
      mintaKonfirmasi("Yakin mendengar?", saatYaMendengarkan, saatTidakMendengarkan);
    } else if (key == '2') {
      mintaKonfirmasi("Yakin menebak?", saatYaMenebak, saatTidakMenebak);
    }
  }
}

// System running indication
void sistemBerjalan() {
  digitalWrite(ledPin, HIGH);
}

// Handle incorrect answer
void jawabanSalah() {
  digitalWrite(buzzerPin, HIGH);
  waktuMulaiBuzzer = millis();
  dfPlayer.play(286); // Play a specific "wrong answer" sound
}

// Display main menu
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

// Listening mode
void modeMendengarkan() {
  #ifdef EXTENDED_LISTENING_MODE
    lcd.clear();
    lcd.print("Pilih metode:");
    lcd.setCursor(0, 1);
    lcd.print("1: Manual");
    lcd.setCursor(0, 3);
    lcd.print("2: Random");

    while (true) {
      char key = keypad.getKey();
      if (key == '1') {
        mendengarkanManual();
        break;
      }
      if (key == '2') {
        mendengarkanRandom();
        break;
      }
    }
  #else
    mendengarkanManual();
  #endif
}

// Manual listening
void mendengarkanManual() {
  lcd.clear();
  lcd.print("Masukkan nomor:");
  char input[4] = "";
  dapatkanInputKeypad(input, sizeof(input));
  nomorFile = atoi(input);

  mintaKonfirmasi("Putar file ini?", saatPutarFile, tampilkanMenuUtama);
}

// Random listening
void mendengarkanRandom() {
  nomorFile = random(1, 287);
  saatPutarFile();
}

// Guessing mode
void modeMenebak() {
  nomorFile = random(1, 287);
  playAudioFile(nomorFile);
  audioSedangDiputar = true;
  mintaKonfirmasi("Tebak nomor?", saatTebakanJawaban, tampilkanMenuUtama);
}

// Request confirmation
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

// Get keypad input
void dapatkanInputKeypad(char* bufferInput, size_t ukuranBuffer) {
  size_t indeks = 0;
  while (indeks < ukuranBuffer - 1) {
    char key = keypad.getKey();
    if (key) {
      bufferInput[indeks++] = key;
      lcd.setCursor(indeks, 1);
      lcd.print(key);
    }
  }
  bufferInput[indeks] = '\0';
}

// Play audio file based on track number
void playAudioFile(int trackNumber) {
  dfPlayer.play(trackNumber);
  audioSedangDiputar = true;
  lcd.clear();
  lcd.print("Memutar file ");
  lcd.setCursor(0, 1);
  lcd.print(trackNames[trackNumber - 1]);  // Display the track name
}

// Callback functions
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
    dfPlayer.play(287); // Play a specific "correct answer" sound
    tampilkanPesan(("Benar! Skor: " + String(skor)).c_str());
  } else {
    skor--;
    dfPlayer.play(286); // Play a specific "wrong answer" sound
    jawabanSalah();
    tampilkanPesan(("Salah! Skor: " + String(skor)).c_str());
  }

  if (skor == 0) {
    tampilkanPesan("Permainan Selesai");
    dfPlayer.play(288); // Play a specific "game over" sound
  } else if (skor == 10) {
    tampilkanPesan("Anda Menang!");
    dfPlayer.play(289); // Play a specific "winning" sound
  } else {
    mintaKonfirmasi("Main lagi?", saatYaMendengarkan, tampilkanMenuUtama);
  }
}

// Display a temporary message
void tampilkanPesan(const char* pesan) {
  lcd.clear();
  lcd.print(pesan);
  waktuPesanMulai = millis();
  pesanAktif = true;
}

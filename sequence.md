sequenceDiagram
    participant User
    participant Arduino
    participant LCD
    participant Keypad
    participant DFPlayer
    participant Buzzer
    participant LED

    rect rgb(191, 223, 255)
    User->>Keypad: Press Key
    Keypad->>Arduino: Send Key Value
    Arduino->>LCD: Display Main Menu
    end

    rect rgb(255, 223, 191)
    User->>Keypad: Press 1 (Listening Mode)
    Keypad->>Arduino: Send Key Value
    Arduino->>User: Request Confirmation (Listening Mode)
    User->>Keypad: Press Y (Yes)
    Keypad->>Arduino: Send Key Value
    Arduino->>LCD: Display Listening Mode Menu
    end

    rect rgb(191, 255, 223)
    User->>Keypad: Press 1 or 2 (Manual/Random)
    Keypad->>Arduino: Send Key Value
    alt Manual Mode
        Arduino->>User: Request Track Number
        User->>Keypad: Enter Track Number
        Keypad->>Arduino: Send Track Number
        Arduino->>User: Confirm to Play Track
        User->>Keypad: Press Y (Yes)
        Keypad->>Arduino: Send Key Value
    else Random Mode
        Arduino->>Arduino: Generate Random Track Number
    end
    end

    rect rgb(223, 191, 255)
    Arduino->>DFPlayer: Play Track
    DFPlayer->>Arduino: Playing Status
    Arduino->>LCD: Display Playing Status
    DFPlayer->>Arduino: Track Finished
    Arduino->>LCD: Display Main Menu
    end

    rect rgb(255, 223, 191)
    User->>Keypad: Press 2 (Guessing Mode)
    Keypad->>Arduino: Send Key Value
    Arduino->>User: Request Confirmation (Guessing Mode)
    User->>Keypad: Press Y (Yes)
    Keypad->>Arduino: Send Key Value
    Arduino->>Arduino: Generate Random Track Number
    Arduino->>DFPlayer: Play Track
    DFPlayer->>Arduino: Playing Status
    Arduino->>LCD: Display Playing Status
    DFPlayer->>Arduino: Track Finished
    Arduino->>User: Request Track Guess
    User->>Keypad: Enter Track Number
    Keypad->>Arduino: Send Track Number
    alt Correct Guess
        Arduino->>DFPlayer: Play Correct Sound
        Arduino->>User: Display Correct Message
        Arduino->>Arduino: Increment Score
    else Incorrect Guess
        Arduino->>DFPlayer: Play Incorrect Sound
        Arduino->>Buzzer: Activate Buzzer
        Arduino->>User: Display Incorrect Message
        Arduino->>Arduino: Decrement Score
    end
    end

    rect rgb(191, 255, 223)
    Arduino->>LCD: Display Main Menu or Game Over/Win
    end

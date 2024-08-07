flowchart TD
 subgraph Setup["Setup"]
        A1["Initialize LCD"]
        A2["Initialize Serial"]
        A3["Initialize dfPlayer"]
        A4["Check dfPlayer Connection"]
  end
 subgraph MainLoop["MainLoop"]
        B1["Check Buzzer Timeout"]
        B2["Check Audio Playback Status"]
        B3["Check Keypad Input"]
        B4["Check Message Display Duration"]
        C{"Buzzer Timeout?"}
        B["Main Loop"]
        D["Turn off Buzzer"]
        E{"Audio Playback Finished?"}
        F["Display Main Menu"]
        G{"Keypad Input?"}
        H["Process Keypad Input"]
        I{"Message Active?"}
        J["Check Message Duration"]
        K{"Key: 1 or 2"}
        L["Confirm Listening Mode"]
        M["Confirm Guessing Mode"]
        N["Enter Listening Mode"]
        O["Enter Guessing Mode"]
  end
 subgraph ListeningMode["ListeningMode"]
        N1["Display Listening Menu"]
        N2{"Select Method"}
        Q["Manual Listening"]
        R["Random Listening"]
        S["Enter Track Number"]
        T["Confirm Play"]
        U["Play Track"]
  end
 subgraph GuessingMode["GuessingMode"]
        O1["Select Random Track"]
        O2["Confirm Guess"]
        X["Enter Guess"]
        Y{"Correct Guess?"}
        Z["Increase Score, Play Correct Sound"]
        AA["Decrease Score, Play Wrong Sound"]
        AB{"Score: 10?"}
        AC["Display Win Message, Play Win Sound"]
        AD["Continue Guessing?"]
        AE{"Score: 0?"}
        AF["Display Game Over Message, Play Game Over Sound"]
  end
    A1 --> A2
    A2 --> A3
    A3 --> A4
    A4 -- Success --> B
    A4 -- Failure --> A5["Display Error, Halt"]
    B --> B1
    B1 --> C
    C -- Yes --> D
    C -- No --> E
    E -- Yes --> F
    E -- No --> G
    G -- Yes --> H
    G -- No --> I
    I -- Yes --> J
    J -- Timeout --> F
    J -- Not Timeout --> B
    I -- No --> B
    H --> K
    K -- 1 --> L
    K -- 2 --> M
    K -- Other --> B
    L -- Yes --> N
    L -- No --> F
    M -- Yes --> O
    M -- No --> F
    N --> N1
    N1 -- with extended --> N2
    N2 -- Manual --> Q
    N2 -- Random --> R
    Q --> S
    S --> T
    T -- Yes --> U
    T -- No --> F
    R --> U
    O --> O1
    O1 --> O2
    O2 -- Yes --> X
    O2 -- No --> F
    X --> Y
    Y -- Yes --> Z
    Y -- No --> AA
    Z --> AB & B
    AB -- Yes --> AC
    AB -- No --> AD
    AD -- Yes --> O
    AD -- No --> F
    AA --> AE & B
    AE -- Yes --> AF
    AE -- No --> AD
    F --> B
    D --> B
    U --> B
    N1 -. no extended .-> Q
     A1:::mainComp
     A2:::mainComp
     A3:::mainComp
     A4:::condNode
     B1:::procNode
     B2:::procNode
     B3:::procNode
     B4:::procNode
     C:::condNode
     B:::mainComp
     D:::procNode
     E:::condNode
     F:::procNode
     G:::condNode
     H:::procNode
     I:::condNode
     J:::procNode
     K:::condNode
     L:::procNode
     M:::procNode
     N:::procNode
     O:::procNode
     N1:::subComp
     N2:::condNode
     Q:::procNode
     R:::procNode
     S:::procNode
     T:::procNode
     U:::procNode
     O1:::subComp
     O2:::condNode
     X:::procNode
     Y:::condNode
     Z:::procNode
     AA:::procNode
     AB:::condNode
     AC:::procNode
     AD:::condNode
     AE:::condNode
     AF:::procNode
     A5:::procNode
    classDef bgColor fill:#fff9f4,stroke:#000,stroke-width:2px
    classDef mainComp fill:#ffcccc,stroke:#000,stroke-width:2px
    classDef subComp fill:#cce7ff,stroke:#000,stroke-width:2px
    classDef condNode fill:#f2e6ff,stroke:#000,stroke-width:2px
    classDef procNode fill:#e6ffe6,stroke:#000,stroke-width:2px


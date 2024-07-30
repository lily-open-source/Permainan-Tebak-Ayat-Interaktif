flowchart TD
    %% Style for milky white background
    classDef bgColor fill:#fff9f4,stroke:#000,stroke-width:2px;

    %% Style for main components
    classDef mainComp fill:#ffcccc,stroke:#000,stroke-width:2px;

    %% Style for sub-components
    classDef subComp fill:#cce7ff,stroke:#000,stroke-width:2px;

    %% Style for conditional nodes
    classDef condNode fill:#f2e6ff,stroke:#000,stroke-width:2px;

    %% Style for process nodes
    classDef procNode fill:#e6ffe6,stroke:#000,stroke-width:2px;

    subgraph Setup
        A1[Initialize LCD]:::mainComp
        A2[Initialize Serial]:::mainComp
        A3[Initialize dfPlayer]:::mainComp
        A4[Check dfPlayer Connection]:::condNode
        A1 --> A2 --> A3 --> A4
    end
    class Setup bgColor;

    A4 -->|Success| B[Main Loop]:::mainComp
    A4 -->|Failure| A5[Display Error, Halt]:::procNode

    subgraph MainLoop
        B1[Check Buzzer Timeout]:::procNode
        B2[Check Audio Playback Status]:::procNode
        B3[Check Keypad Input]:::procNode
        B4[Check Message Display Duration]:::procNode
        B --> B1 --> C{Buzzer Timeout?}:::condNode
        C -->|Yes| D[Turn off Buzzer]:::procNode
        C -->|No| E{Audio Playback Finished?}:::condNode
        E -->|Yes| F[Display Main Menu]:::procNode
        E -->|No| G{Keypad Input?}:::condNode
        G -->|Yes| H[Process Keypad Input]:::procNode
        G -->|No| I{Message Active?}:::condNode
        I -->|Yes| J[Check Message Duration]:::procNode
        J -->|Timeout| F
        J -->|Not Timeout| B
        I -->|No| B
        H --> K{Key: 1 or 2}:::condNode
        K -->|1| L[Confirm Listening Mode]:::procNode
        K -->|2| M[Confirm Guessing Mode]:::procNode
        K -->|Other| B
        L -->|Yes| N[Enter Listening Mode]:::procNode
        L -->|No| F
        M -->|Yes| O[Enter Guessing Mode]:::procNode
        M -->|No| F
    end
    class MainLoop bgColor;

    subgraph ListeningMode
        N1[Display Listening Menu]:::subComp
        N2{Select Method}:::condNode
        N --> N1 --> N2
        N2 -->|Manual| Q[Manual Listening]:::procNode
        N2 -->|Random| R[Random Listening]:::procNode
        Q --> S[Enter Track Number]:::procNode
        S --> T[Confirm Play]:::procNode
        T -->|Yes| U[Play Track]:::procNode
        T -->|No| F
        R --> U
    end
    class ListeningMode bgColor;

    subgraph GuessingMode
        O1[Select Random Track]:::subComp
        O2[Confirm Guess]:::condNode
        O --> O1 --> O2
        O2 -->|Yes| X[Enter Guess]:::procNode
        O2 -->|No| F
        X --> Y{Correct Guess?}:::condNode
        Y -->|Yes| Z[Increase Score, Play Correct Sound]:::procNode
        Y -->|No| AA[Decrease Score, Play Wrong Sound]:::procNode
        Z --> AB{Score: 10?}:::condNode
        AB -->|Yes| AC[Display Win Message, Play Win Sound]:::procNode
        AB -->|No| AD[Continue Guessing?]:::condNode
        AD -->|Yes| O
        AD -->|No| F
        AA --> AE{Score: 0?}:::condNode
        AE -->|Yes| AF[Display Game Over Message, Play Game Over Sound]:::procNode
        AE -->|No| AD
    end
    class GuessingMode bgColor;

    F[Display Main Menu] --> B
    D --> B
    U --> B
    Z --> B
    AA --> B

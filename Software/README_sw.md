# Anzeigetafel - Football Scoreboard Application - Software

A modular Qt application to manage and display live football match events. Developed as a mini-project for the BTE5058a "Einstieg in OOP" course at BFH.

## Author

**Paranithan Paramalingam**  
Bachelor of Engineering, BFH  
Course: BTE5058a – Einstieg in OOP  
Submission date: 16 June 2025

> This project was developed with the assistance of ChatGPT (OpenAI) for code generation and documentation.

## Project Overview

This application provides a **control window** for match operators and a **scoreboard window** for public display. It tracks players, goals, and match time, offering real-time updates via Qt's signal-slot mechanism.

### Control Window

![Controll window](Dokumentationen/Bilder/Controll_Window.png)

### Scoreboard Window

![Scoreboard window](Dokumentationen/Bilder/score_board.png)

### Key Features

- Two synchronized windows: Control (operator) & Scoreboard (audience)
- Dynamic team roster management (add/remove players)
- Goal logging with timestamp and own goal detection
- Match timer with 1st and 2nd half support (00:00–45:00 and 45:00–90:00)
- CSV import for team lineups
- Emblem/logo support for team branding
- Undo last goal, reset score, and timer restart
- Fullscreen toggle for the scoreboard

<<<<<<< Updated upstream
## Architecture

=======
>>>>>>> Stashed changes
### Class Diagram

![Class Diagram](Dokumentationen/Bilder/KlassenDiagramm_Endversion.drawio.png)

### Data Flow Diagram

![Data Flow Diagram](Dokumentationen/Bilder/DFTDiagramm.png)

### Main Components

- `Person` – Base class for player objects
- `home_team` / `away_team` – Manage team-specific players
- `score_memory` – Stores goals, scores, and timestamps
- `timer` – Manages game time and handles period transitions
- `controll_window` – Operator interface for managing events
- `score_board` – Visual display of score, time, and goals

### Signal-Slot Connections

- Goal updates trigger visual refresh in both windows
- Timer emits updated time every second
- User actions (e.g. undo goal, import team) affect game state instantly

## Build & Run Instructions

### Requirements

- Qt 6.6 or newer
- C++17 compatible compiler
- CMake or Qt Creator (without `.ui` files)

### Build with Qt Creator

1. Open `CMakeLists.txt` or create a new CMake project and add all `.cpp` and `.h` files.
2. Configure and run the project using Qt Creator.
3. No `.ui` files are used — all UI elements are constructed programmatically.

## Limitations

- Fixed to 2x 45-minute halfes
- No support for extra time or custom periods, After 45 minutes the timer starts from 46th minute to 90th minute. After 90 minutes the timer starts from 00:00
- No duplicate number validation for player lists
- No persistent storage or game state saving

# Anzeigetafel - Football Scoreboard Application - Software

A modular Qt application to manage and display live football match events. Developed as a mini-project for the BTE5058a "Einstieg in OOP" course at BFH.

## Author

**Paranithan Paramalingam**  
Bachelor of Engineering, BFH  
Course: BTE5512 – Projektarbeit | BTE5540 - Bachelor Thesis  
Version: 2.1, December 2025

> This project was developed with the assistance of AI tools (ChatGPT/GitHub Copilot) for code generation and documentation.

## Project Overview

This application provides a **control window** for match operators and a **scoreboard window** for public display. It tracks players, goals, and match time, offering real-time updates via Qt's signal-slot mechanism.

### Control Window

![Controll window](Dokumentationen/Bilder/Controll_Window.png)

_Operator interface for managing teams, goals, timer, and match state._

#### Scoreboard Display

![Scoreboard](Dokumentationen/Bilder/score_board.png)

_Fullscreen audience display with score, time, emblems, and goal scorers._

### Key Features

- **Two Synchronized Windows**: Control (operator) & Scoreboard (audience display)
- **Dynamic Team Roster Management**: Add/remove players, CSV import with UTF-8 support
- **Goal Logging**: Player attribution, timestamp recording, own goal detection
- **Match Timer**: 1st and 2nd half tracking (00:00–45:00 and 45:00–90:00)
- **Match State Machine**: PreGame, FirstHalf, HalfTime, SecondHalf, PostGame phases
- **Slideshow Support**: Automatically displays slides during PreGame/HalfTime/PostGame states
- **Team Emblems**: Upload and display team logos/crests
- **Undo/Reset Functions**: Remove last goal, reset entire match state
- **Fullscreen Toggle**: Double-click to show/hide windows
- **Qt Signal-Slot Architecture**: Real-time synchronization between all components

### Class Diagram

![Class Diagram](../documentation/class_diagram.drawio)

_Complete class structure showing inheritance, composition, and relationships._

### Data Flow Diagram

![Data Flow Diagram](../documentation/DFTDiagramm.drawio)

_Data flow between external inputs, processing units, data storage, and display outputs._

### Main Components

- **`player`** – Player data model with number and name
- **`home_team` / `away_team`** – Team roster management with player collections
- **`score_memory`** – Central data store for goals, scores, timestamps, and player info
- **`timer`** – Phase-aware match timer with automatic 45-minute limit
- **`controll_window`** – Operator interface with team/goal/timer/state management
- **`Score_board`** – Fullscreen display with dynamic font scaling and slideshow support

### Signal-Slot Connections

- **Goal Updates**: `score_memory::goalsUpdated()` → `Score_board::updateGoalList()` + `controll_window::UpdateScoreDisplay()`
- **Timer Updates**: `timer::timeUpdated(QString)` → `Score_board::updateTimeDisplay(QString)`
- **Match State Changes**: `controll_window::matchStateChanged()` → `Score_board::updateMatchState()`
- **Emblem Changes**: `controll_window::emblemChanged()` → `Score_board::updateEmblem()`
- **Window Toggle**: Double-click events on either window show/hide the other

## Build & Run Instructions

### Requirements

- **Qt**: 6.6 or newer (tested with Qt 6.10)
- **Compiler**: C++17 compatible (MinGW 64-bit, MSVC, or GCC)
- **Build System**: CMake 3.30+ or Qt Creator
- **Platform**: Windows, Linux, or macOS

### Build with Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Select a kit (Desktop Qt 6.x MinGW 64-bit or equivalent)
3. Configure the project (CMake will auto-generate build files)
4. Build and run

### Build with CMake (Command Line)

```bash
mkdir build && cd build
cmake ..
cmake --build .
./anzeigetafel  # or anzeigetafel.exe on Windows
```

### Project Structure

```
software/
├── CMakeLists.txt           # Build configuration
├── main.cpp                 # Application entry point
├── resources.qrc            # Qt resource file (icons)
├── include/                 # Header files
│   ├── controll_window.h
│   ├── score_board.h
│   ├── timer.h
│   ├── score_memory.h
│   ├── home_team.h
│   ├── away_team.h
│   └── player.h
└── src/                     # Implementation files
    ├── controll_window.cpp
    ├── score_board.cpp
    ├── timer.cpp
    ├── score_memory.cpp
    ├── home_team.cpp
    ├── away_team.cpp
    └── player.cpp
```

**Note**: All UI elements are constructed programmatically (no `.ui` files).

## Known Limitations

- **Fixed Match Duration**: Designed for 2×45 minute football matches only
- **Timer Behavior**: Automatically stops at 45:00 per half; manual restart required for second half
- **No Extra Time**: No support for injury time, extra time, or penalty shootouts
- **Player Validation**: No duplicate jersey number checking
- **No Persistence**: Match state is not saved to disk (restarting clears all data)
- **Slideshow Management**: Slides must be manually placed in `slides/PreGame`, `slides/HalfTime`, and `slides/PostGame` folders
- **Single Match Mode**: Cannot manage multiple matches simultaneously

## Future Enhancements

- Database integration for match history and statistics
- Network broadcasting for remote displays
- Configurable match durations (rugby, basketball, etc.)
- Live commentary/event log export
- Multi-language UI support

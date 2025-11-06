cmake_minimum_required(VERSION 3.16)
project(Anzeigetafel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic MOC/UIC/RCC steps
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt (Qt6 preferred, falls back to Qt5)
find_package(QT NAMES Qt6 Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ---- List your sources explicitly (edit to match your filenames) ----
add_executable(anzeigetafel
    main.cpp
    controll_window.cpp
    score_board.cpp
    score_memory.cpp
    Person.cpp
    home_team.cpp
    away_team.cpp
    timer.cpp
)

# --- Add icon resources ---
qt_add_resources(anzeigetafel "app_resources"
    PREFIX "/"
    FILES resources.qrc
)


# If your headers live in this folder:
target_include_directories(anzeigetafel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link Qt Widgets
target_link_libraries(anzeigetafel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Optional: install target (nice for 'make install')
install(TARGETS anzeigetafel RUNTIME DESTINATION bin)

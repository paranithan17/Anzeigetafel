cmake_minimum_required(VERSION 3.16)
project(Anzeigetafel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic MOC/UIC/RCC steps
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt (Qt6 preferred, falls back to Qt5)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets WebSockets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Pdf QUIET)

if (TARGET Qt${QT_VERSION_MAJOR}::Pdf)
    target_compile_definitions(anzeigetafel PRIVATE HAVE_QTPDF=1)
endif()

# ---- List your sources explicitly (match new folder structure) ----
add_executable(anzeigetafel
    main.cpp
    src/controll_window.cpp
    src/score_board.cpp
    src/score_memory.cpp
    src/player.cpp
    src/team.cpp
    src/timer.cpp
    src/match_controller.cpp
    src/websocket.cpp
    include/controll_window.h
    include/score_board.h
    include/score_memory.h
    include/player.h
    include/team.h
    include/timer.h
    include/match_controller.h
    include/websocket.h
    resources.qrc
)

# If your headers live in the include folder:
target_include_directories(anzeigetafel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Qt Widgets and WebSockets
target_link_libraries(anzeigetafel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebSockets)

if (TARGET Qt${QT_VERSION_MAJOR}::Pdf)
    target_link_libraries(anzeigetafel PRIVATE Qt${QT_VERSION_MAJOR}::Pdf)
endif()

# Optional: install target (nice for 'make install')
install(TARGETS anzeigetafel RUNTIME DESTINATION bin)


/**
 * @file score_board.h
 * @brief Scoreboard display window for live match information.
 *
 * Displays live score, match time, and goal scorer information to audience.
 * Supports fullscreen mode, team emblems, and state-based content (slides for
 * PreGame/HalfTime/PostGame, match stats for FirstHalf/SecondHalf).
 *
 * Features:
 * - Fullscreen display with frameless window.
 * - Dynamic font scaling based on window size.
 * - Scoreboards (team emblems, score, time, goal list).
 * - Slide show for non-game states.
 * - Double-click toggle to hide/show control window.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.1, 2025-12-25
 */
#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>
#include <QDir>
#include <QPixmap>
#include <QMouseEvent>
#include <QFileInfo>

#include "score_memory.h"
#include "timer.h"

class Score_board : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Match state enumeration synchronized with controll_window.
     *
     * Defines the current phase of the match to control display mode.
     * @see controll_window::MatchState
     */
    enum class MatchState
    {
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };

    /**
     * @brief Constructs the scoreboard display window.
     *
     * Initializes fullscreen display with score, time, and goal lists.
     * Sets up slideshow system for non-game states and connects to score/timer updates.
     *
     * @param scoreMemory Pointer to score data manager
     * @param gameTime Pointer to match timer
     * @param parent Parent widget (default nullptr)
     */
    Score_board(score_memory *scoreMemory, timer *gameTime, QWidget *parent = nullptr);

    /**
     * @brief Sets reference to control window for toggle functionality.
     *
     * Allows double-click on scoreboard to show/hide operator control window.
     *
     * @param window Pointer to control window widget
     */
    void setControlWindow(QWidget *window);

private:
    MatchState m_state = MatchState::PreGame;
    score_memory *Score;
    timer *gameTime;
    QWidget *controlWindow = nullptr;

    QLabel *scoreLabel;
    QLabel *timeLabel;
    QListWidget *scorerListTeam1;
    QListWidget *scorerListTeam2;
    QLabel *emblemTeam1;
    QLabel *emblemTeam2;

    QTimer *slideshowTimer = nullptr;
    QLabel *slideshowLabel = nullptr;
    QStringList slideshowFiles;
    int slideshowIndex = 0;

	// Windows path
    //QString preGamePath = "C:/Users/paran/Desktop/Anzeigetafel/slides/PreGame";
    //QString halfTimePath = "C:/Users/paran/Desktop/Anzeigetafel/slides/HalfTime";
    //QString postGamePath = "C:/Users/paran/Desktop/Anzeigetafel/slides/PostGame";
    
    // Linux path
    QString preGamePath  = "/home/rpi/Anzeigetafel/slides/PreGame";
    QString halfTimePath = "/home/rpi/Anzeigetafel/slides/HalfTime";
    QString postGamePath = "/home/rpi/Anzeigetafel/slides/PostGame";

    /**
     * @brief Initializes UI layout with score, time, and goal lists.
     */
    void setupLayout();

    /**
     * @brief Applies stylesheet (colors, fonts) to UI elements.
     */
    void applyStyle();

    /**
     * @brief Adjusts font sizes based on window dimensions.
     *
     * Scales dynamically for fullscreen or uses fixed sizes for LED wall.
     */
    void adjustFontSize();

    /**
     * @brief Adjusts emblem sizes based on window dimensions.
     */
    void adjustEmblemSize();

    /**
     * @brief Updates display mode based on current match state.
     *
     * Shows scoreboard for FirstHalf/SecondHalf, slideshow for other states.
     */
    void updateViewForState();

    /**
     * @brief Starts slideshow from specified folder.
     *
     * @param folderPath Path to folder containing slide images
     */
    void startSlideshow(const QString &folderPath);

    /**
     * @brief Stops active slideshow and hides slideshow label.
     */
    void stopSlideshow();

    /**
     * @brief Displays next slide in slideshow sequence.
     */
    void showNextSlide();

    /**
     * @brief Collects all slide files from folder.
     *
     * @param folderPath Path to folder containing slides
     * @return List of absolute file paths to slide images
     */
    QStringList collectSlides(const QString &folderPath);

    /**
     * @brief Collects image files from directory.
     *
     * @param dir Directory to scan for images
     * @return List of absolute file paths to images
     */
    QStringList collectImages(const QDir &dir);

private slots:
    /**
     * @brief Updates score display from score memory.
     *
     * Refreshes "Home : Away" score text.
     */
    void updateScore();

    /**
     * @brief Populates goal scorer lists for both teams.
     *
     * Displays player numbers, names, timestamps, and marks own goals.
     *
     * @param goals List of goals to display
     */
    void extracted(QList<Goal> &goals);

    /**
     * @brief Updates goal scorer lists from score memory.
     *
     * Clears and repopulates both team scorer lists.
     */
    void updateGoals();

    /**
     * @brief Updates time display from timer.
     *
     * @param time Formatted time string (MM:SS)
     */
    void updateTime(const QString &time);

protected:
    /**
     * @brief Handles keyboard events (ESC to exit fullscreen).
     *
     * @param event Keyboard event
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Handles window resize events.
     *
     * Adjusts font sizes and emblem scaling based on new window dimensions.
     * Updates slideshow label geometry to cover full window.
     *
     * @param event Resize event
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handles double-click to toggle control window.
     *
     * @param event Mouse event
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

public slots:
    /**
     * @brief Updates team emblem display.
     *
     * Loads and displays team emblem image from file path.
     * Supports dynamic emblem updates during match setup.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param filePath Absolute path to emblem image file
     */
    void updateEmblem(const QString &team, const QString &filePath);

    /**
     * @brief Changes match state and updates display mode.
     *
     * Switches between scoreboard view (FirstHalf/SecondHalf) and
     * slideshow mode (PreGame/HalfTime/PostGame).
     *
     * @param state Integer representation of MatchState (0-4)
     * @see MatchState
     */
    void setMatchState(int state);

    /**
     * @brief Toggles visibility of control window.
     *
     * Shows control window if hidden, hides if visible.
     * Triggered by double-click on scoreboard.
     */
    void toggleControlWindow();
};
#endif // SCORE_BOARD_H

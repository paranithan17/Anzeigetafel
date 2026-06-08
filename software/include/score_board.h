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
 * @version 3.0, 2026-04-13
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
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>

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
        /** @brief Before kickoff, setup/presentation phase. */
        PreGame = 0,
        /** @brief First half running phase. */
        FirstHalf,
        /** @brief Halftime break phase. */
        HalfTime,
        /** @brief Second half running phase. */
        SecondHalf,
        /** @brief Match finished phase. */
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
     * @return void
     */
    Score_board(score_memory *scoreMemory, timer *gameTime, QWidget *parent = nullptr);

    /**
     * @brief Sets reference to control window for toggle functionality.
     *
     * Allows double-click on scoreboard to show/hide operator control window.
     *
     * @param window Pointer to control window widget
     * @return void
     */
    void setControlWindow(QWidget *window);

private:
    /** @brief Current scoreboard display state. */
    MatchState m_state = MatchState::PreGame;
    /** @brief Score memory/data source reference. */
    score_memory *Score;
    /** @brief Match timer reference for elapsed-time updates. */
    timer *gameTime;
    /** @brief Optional control window reference for visibility toggle. */
    QWidget *controlWindow = nullptr;

    /** @brief Label showing current score line. */
    QLabel *scoreLabel;
    /** @brief Label showing formatted match time. */
    QLabel *timeLabel;
    /** @brief Goal scorer list widget for home team. */
    QListWidget *scorerListTeam1;
    /** @brief Goal scorer list widget for away team. */
    QListWidget *scorerListTeam2;
    /** @brief Emblem display label for home team. */
    QLabel *emblemTeam1;
    /** @brief Emblem display label for away team. */
    QLabel *emblemTeam2;

    /** @brief Timer driving automatic slide transitions. */
    QTimer *slideshowTimer = nullptr;
    /** @brief Full-window label used for slideshow rendering. */
    QLabel *slideshowLabel = nullptr;
    struct SlidePage
    {
        QString filePath;
        int pageNumber = 0;
        bool isPdf = false;
    };

    /** @brief Ordered slide entries used for the slideshow. */
    QList<SlidePage> slideshowPages;
    /** @brief Current zero-based slideshow index. */
    int slideshowIndex = 0;

    QString baseSlidePath;
    QString preGamePath;
    QString halfTimePath;
    QString postGamePath;

    QString findSlideBasePath();
    void setupSlidePaths();

    /**
     * @brief Initializes UI layout with score, time, and goal lists.
     * @return void
     */
    void setupLayout();

    /**
     * @brief Applies stylesheet (colors, fonts) to UI elements.
     * @return void
     */
    void applyStyle();

    /**
     * @brief Adjusts font sizes based on window dimensions.
     *
     * Scales dynamically for fullscreen or uses fixed sizes for LED wall.
     *
     * @return void
     */
    void adjustFontSize();

    /**
     * @brief Adjusts emblem sizes based on window dimensions.
     * @return void
     */
    void adjustEmblemSize();

    /**
     * @brief Updates display mode based on current match state.
     *
     * Shows scoreboard for FirstHalf/SecondHalf, slideshow for other states.
     *
     * @return void
     */
    void updateViewForState();

    /**
     * @brief Starts slideshow from specified folder.
     *
     * @param folderPath Path to folder containing slides
     * @return void
     */
    void startSlideshow(const QString &folderPath);

    /**
     * @brief Stops active slideshow and hides slideshow label.
     * @return void
     */
    void stopSlideshow();

    /**
     * @brief Displays next slide in slideshow sequence.
     * @return void
     */
    void showNextSlide();

    /**
     * @brief Adujusts scorer list widgets dependon on longest name
     * @return void
     */
    void adjustScorerListWidths();

    /**
     * @brief Collects all slide files from folder.
     *
     * @param folderPath Path to folder containing slides
     * @return List of absolute slide entries
     */
    QList<SlidePage> collectSlides(const QString &folderPath);

    /**
     * @brief Collects PDF pages and image files from directory.
     *
     * @param dir Directory to scan for slides
     * @return List of absolute file paths and page numbers
     */
    QList<SlidePage> collectPdfPages(const QDir &dir);

private slots:
    /**
     * @brief Updates score display from score memory.
     *
     * Refreshes "Home : Away" score text.
     *
     * @return void
     */
    void updateScore();

    /**
     * @brief Populates goal scorer lists for both teams.
     *
     * Displays player numbers, names, timestamps, and marks own goals.
     *
     * @param goals List of goals to display
     * @return void
     */
    void extracted(QList<Goal> &goals);

    /**
     * @brief Updates goal scorer lists from score memory.
     *
     * Clears and repopulates both team scorer lists.
     *
     * @return void
     */
    void updateGoals();

    /**
     * @brief Updates time display from timer.
     *
     * @param time Formatted time string (MM:SS)
     * @return void
     */
    void updateTime(const QString &time);

protected:
    /**
     * @brief Handles keyboard events (ESC to exit fullscreen).
     *
     * @param event Keyboard event
     * @return void
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Handles window resize events.
     *
     * Adjusts font sizes and emblem scaling based on new window dimensions.
     * Updates slideshow label geometry to cover full window.
     *
     * @param event Resize event
     * @return void
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handles double-click to toggle control window.
     *
     * @param event Mouse event
     * @return void
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
     * @return void
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
     * @return void
     */
    void setMatchState(int state);

    /**
     * @brief Toggles visibility of control window.
     *
     * Shows control window if hidden, hides if visible.
     * Triggered by double-click on scoreboard.
     *
     * @return void
     */
    void toggleControlWindow();
};
#endif // SCORE_BOARD_H

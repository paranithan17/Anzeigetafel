/**
 * @file controll_window.h
 * @brief Control window for match operator to manage teams, scorers, and timer.
 *
 * Provides interface for adding/removing players, logging goals, controlling
 * the match timer, and managing match state transitions. Synchronizes with
 * scoreboard display and score memory.
 *
 * Features:
 * - Team roster management (add/remove players, CSV import with UTF-8).
 * - Goal logging with player selection and own-goal detection.
 * - Match timer control (start/stop/restart) with state validation.
 * - Match state machine (PreGame, FirstHalf, HalfTime, SecondHalf, PostGame).
 * - Team emblems management.
 * - Undo/reset functionality via Log dialog.
 * - Double-click toggle to hide/show scoreboard.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.1, 2025-12-25
 */
#ifndef CONTROLL_WINDOW_H
#define CONTROLL_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QGroupBox>
#include <QMouseEvent>

#include "home_team.h"
#include "away_team.h"
#include "timer.h"
#include "score_memory.h"

class controll_window : public QWidget
{
    Q_OBJECT

public:
    // Simple state machine for the match
    enum class MatchState
    {
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };

    /**
     * @brief Constructor for the Control Window.
     *
     * Initializes the operator control interface with team management, goal logging,
     * timer control, and match state selection. Creates UI elements for adding/removing
     * players, importing team rosters from CSV, logging goals with player selection,
     * uploading team emblems, and managing match phases (PreGame → FirstHalf → HalfTime
     * → SecondHalf → PostGame).
     *
     * @param parent Parent widget pointer (default: nullptr)
     *
     * @note Constructs all UI elements and connects signals/slots for internal state
     *       management. Does NOT require initial connections to score_memory, timer,
     *       or Score_board; these are set later via setScoreMemory(), setTimer(), etc.
     */
    controll_window(QWidget *parent = nullptr);

    /**
     * @brief Initializes the control window with a score memory instance.
     *
     * Stores a pointer to the score memory object and connects the goalsUpdated signal
     * to the UpdateScoreDisplay() slot for live score synchronization.
     *
     * @param mem Pointer to the score_memory instance managing goal data.
     *
     * @note Called during main window initialization in main.cpp before setTimer().
     * @note Enables automatic score display updates when goals are logged.
     *
     * @see setTimer() for timer initialization
     * @see score_memory class
     */
    void setScoreMemory(score_memory *mem);

    /**
     * @brief Initializes the control window with a timer instance.
     *
     * Stores a pointer to the timer object and connects timeUpdated and timeout signals
     * to UpdateTimeDisplay() and handleTimerTimeout() slots respectively for live timer
     * synchronization and half-time transition handling.
     *
     * @param t Pointer to the timer instance managing match time.
     *
     * @note Called during main window initialization in main.cpp after setScoreMemory().
     * @note Enables automatic time display updates and half-time notifications.
     *
     * @see setScoreMemory() for score memory initialization
     * @see timer class
     * @see handleTimerTimeout()
     */
    void setTimer(timer *t);

    /**
     * @brief Stores a reference to the scoreboard window for toggling.
     *
     * Saves the scoreboard widget pointer for bidirectional window control.
     * Enables the double-click toggle feature on the control window to show/hide the scoreboard.
     *
     * @param board Pointer to the Score_board widget (audience display window).
     *
     * @note Called during main window initialization in main.cpp.
     * @note Enables toggleScoreboard() functionality.
     *
     * @see toggleScoreboard()
     * @see Score_board::setControlWindow() for reverse reference
     */
    void setScoreboard(QWidget *board);

    /**
     * @brief Toggles the scoreboard window visibility (show ↔ hide).
     *
     * If the scoreboard is currently visible, hides it. If hidden, shows it, brings it to
     * the foreground (raises), and activates the window. This enables smooth window toggling
     * for presentations where the audience display may need to be temporarily hidden.
     *
     * @note Triggered by double-clicking the control window (mouseDoubleClickEvent).
     * @note Safe check: Only toggles if scoreboard pointer is non-null.
     * @note Window management: Uses raise() and activateWindow() for proper focus handling.
     *
     * @see mouseDoubleClickEvent() for double-click trigger
     * @see setScoreboard() for initialization
     */
    void toggleScoreboard();

signals:
    /**
     * @brief Signal emitted when a team emblem is changed.
     *
     * @param team Team designation ("Home" or "Away")
     * @param filepath Absolute path to the selected emblem image file
     *
     * @note Connected to Score_board::updateEmblem() in main.cpp
     */
    void emblemChanged(const QString &team, const QString &filepath);

    /**
     * @brief Signal emitted when the operator confirms a new match state.
     *
     * Notifies other components (e.g., Score_board) of the current match phase.
     *
     * @param state Integer representation of MatchState enum (use static_cast<int>(MatchState))
     *
     * @note Connected to Score_board::setMatchState() in main.cpp
     * @see MatchState enum for possible values
     * @see applyStateSelection() where this signal is emitted
     */
    void matchStateChanged(int state);

protected:
    /**
     * @brief Handles double-click events on the control window.
     *
     * Overrides the Qt mouseDoubleClickEvent to enable toggling the scoreboard window
     * visibility. Any double-click anywhere on the control window triggers the scoreboard
     * toggle. This provides quick access to show/hide the audience display without using
     * a dedicated menu or button.
     *
     * @param event Qt mouse event (unused; location of click not evaluated).
     *
     * @note Event parameter ignored: Double-click anywhere on window triggers toggle.
     * @note Feature: Allows operator to quickly hide scoreboard during setup/breaks.
     * @note Bidirectional: Scoreboard can also toggle control window via double-click.
     *
     * @see toggleScoreboard() for toggle implementation
     * @see Score_board::mouseDoubleClickEvent() for scoreboard side
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief Opens a dialog to add a player to the home team.
     *
     * Displays a modal dialog prompting the operator to enter a player's number and name.
     * On confirmation, validates the input and adds the player to team1. On validation failure,
     * displays an error message. Updates the home team list on success.
     *
     * @note Uses Qt dialogs and signals/slots for user interaction.
     * @note Input validation: number must be a valid integer, name must not be empty.
     *
     * @see AddPlayerTeam2() for away team equivalent
     * @see RemovePlayerTeam1() for removing players
     */
    void AddPlayerTeam1();

    /**
     * @brief Opens a dialog to add a player to the away team.
     *
     * Displays a modal dialog prompting the operator to enter a player's number and name.
     * On confirmation, validates the input and adds the player to team2. On validation failure,
     * displays an error message. Updates the away team list on success.
     *
     * @note Uses Qt dialogs and signals/slots for user interaction.
     * @note Input validation: number must be a valid integer, name must not be empty.
     *
     * @see AddPlayerTeam1() for home team equivalent
     * @see RemovePlayerTeam2() for removing players
     */
    void AddPlayerTeam2();
    /**
     * @brief Removes a selected player from the home team roster.
     *
     * When a home team player in the list is clicked, displays a confirmation dialog.
     * If confirmed, extracts the player number from the list item and removes them from team1.
     * Updates the home team list display after removal.
     *
     * @param item Clicked QListWidgetItem containing the player "[Number] - [Name]" text.
     *
     * @note Connected as a slot to listTeam1's itemClicked signal.
     * @see RemovePlayerTeam2() for away team equivalent
     * @see AddPlayerTeam1() for adding players
     */
    void RemovePlayerTeam1(QListWidgetItem *item);

    /**
     * @brief Removes a selected player from the away team roster.
     *
     * When an away team player in the list is clicked, displays a confirmation dialog.
     * If confirmed, extracts the player number from the list item and removes them from team2.
     * Updates the away team list display after removal.
     *
     * @param item Clicked QListWidgetItem containing the player "[Number] - [Name]" text.
     *
     * @note Connected as a slot to listTeam2's itemClicked signal.
     * @see RemovePlayerTeam1() for home team equivalent
     * @see AddPlayerTeam2() for adding players
     */
    void RemovePlayerTeam2(QListWidgetItem *item);

    /**
     * @brief Opens a dialog to log a goal for the home team.
     *
     * Displays a modal dialog with separate player lists for both teams. Operator selects
     * a player and optionally adjusts the goal time (auto-filled from current timer +1 minute).
     * If the selected player is from the away team (team2), prompts for own goal confirmation.
     * On save, passes the goal data to ScoreMemory with the "Home" team designation.
     *
     * @note Goal time format: minutes as integer (0-90).
     * @note Own goal logic: confirms if away team player is selected.
     * @note Auto-time feature respects current match phase (FirstHalf/SecondHalf).
     *
     * @see AddGoalTeam2() for away team equivalent
     * @see score_memory::addGoal() for goal data persistence
     */
    void AddGoalTeam1();

    /**
     * @brief Opens a dialog to log a goal for the away team.
     *
     * Displays a modal dialog with separate player lists for both teams. Operator selects
     * a player and optionally adjusts the goal time (auto-filled from current timer +1 minute).
     * If the selected player is from the home team (team1), prompts for own goal confirmation.
     * On save, passes the goal data to ScoreMemory with the "Away" team designation.
     *
     * @note Goal time format: minutes as integer (0-90).
     * @note Own goal logic: confirms if home team player is selected.
     * @note Auto-time feature respects current match phase (FirstHalf/SecondHalf).
     *
     * @see AddGoalTeam1() for home team equivalent
     * @see score_memory::addGoal() for goal data persistence
     */
    void AddGoalTeam2();

    /**
     * @brief Starts the match timer if conditions are met.
     *
     * Starts the timer only if the current match state is FirstHalf or SecondHalf.
     * Disables the Start Timer button during execution to prevent duplicate start signals.
     * If the timer is already running, performs no action.
     *
     * @note Timer is managed by the timer class; this method only sends the start signal.
     * @note Button state is managed by applyStateSelection() based on match phase.
     * @note Debug output: logs "Time is running" or "Timer can only start in First/Second Half".
     *
     * @see handleTimerTimeout() for half-time transitions
     * @see applyStateSelection() for state validation
     */
    void StartTime();

    /**
     * @brief Updates the operator's time display label.
     *
     * Receives the current elapsed time from the timer and updates the control window's
     * time label. This is a slot connected to timer::timeUpdated signal for live updates
     * during match play.
     *
     * @param elapsedTime Current elapsed time in format "MM:SS" (e.g., "15:30").
     *
     * @note Connected as a slot to timer::timeUpdated signal in setTimer().
     * @see timer::timeUpdated
     * @see UpdateScoreDisplay() for score updates
     */
    void UpdateTimeDisplay(const QString &elapsedTime);

    /**
     * @brief Updates the operator's score display label.
     *
     * Retrieves current home and away team scores from ScoreMemory and formats them
     * as "[Home] : [Away]" in the control window's score label. This is a slot connected
     * to score_memory::goalsUpdated signal for live updates.
     *
     * @note Connected as a slot to score_memory::goalsUpdated signal in setScoreMemory().
     * @see score_memory::getHomeScore()
     * @see score_memory::getAwayScore()
     * @see UpdateTimeDisplay() for time updates
     */
    void UpdateScoreDisplay();

    /**
     * @brief Opens a dialog with game control options (Restart, Reset, Undo Goal).
     *
     * Displays a modal dialog with three action buttons:
     * - **Restart**: Restarts the timer for the current phase.
     * - **Reset**: Resets both score and timer to match phase start; re-enables Start Timer button.
     * - **Undo Goal**: Removes the last recorded goal from the score memory.
     *
     * Dialog is closed via Cancel button or action selection.
     *
     * @note Restart: Calls timer::restart()
     * @note Reset: Calls timer::resetToPhaseStart() and score_memory::resetGame()
     * @note Undo Goal: Calls score_memory::removeLastGoal()
     * @note Debug output logs which action was selected.
     *
     * @see score_memory::resetGame()
     * @see score_memory::removeLastGoal()
     * @see timer::restart()
     * @see timer::resetToPhaseStart()
     */
    void Log();

    /**
     * @brief Opens a file dialog to select and upload a home team emblem image.
     *
     * Displays a file browser dialog for selecting an image file (PNG, JPG, BMP).
     * On selection, stores the file path and emits the emblemChanged signal with
     * team designation "Home" to notify the scoreboard to display the new emblem.
     *
     * @note Supported formats: PNG, JPG, BMP.
     * @note Emits emblemChanged("Home", filePath) on successful selection.
     * @note Stores file path in emblemTeam1 member variable.
     *
     * @see loadEmblemTeam2() for away team equivalent
     * @see emblemChanged signal
     */
    void loadEmblemTeam1();

    /**
     * @brief Opens a file dialog to select and upload an away team emblem image.
     *
     * Displays a file browser dialog for selecting an image file (PNG, JPG, BMP).
     * On selection, stores the file path and emits the emblemChanged signal with
     * team designation "Away" to notify the scoreboard to display the new emblem.
     *
     * @note Supported formats: PNG, JPG, BMP.
     * @note Emits emblemChanged("Away", filePath) on successful selection.
     * @note Stores file path in emblemTeam2 member variable.
     *
     * @see loadEmblemTeam1() for home team equivalent
     * @see emblemChanged signal
     */
    void loadEmblemTeam2();

    /**
     * @brief Imports the home team player roster from a CSV file.
     *
     * Opens a file browser dialog to select a CSV file. Reads the file with UTF-8 encoding
     * (supporting German umlauts ä, ö, ü) and parses player data. Supports dynamic delimiter
     * detection: semicolon (;) or comma (,). Each line must have format: "[Number];[Name]".
     * Parses and adds each player to team1, then updates the team list display.
     * On file error, displays a warning message.
     *
     * @note CSV delimiter: Semicolon (;) preferred; comma (,) as fallback.
     * @note UTF-8 encoding: Preserves German umlauts and accents.
     * @note Input validation: Number must be integer; name must not be empty.
     * @note File extension: .csv or .CSV
     *
     * @see ImportTeam2() for away team equivalent
     * @see updateTeamList1() for display refresh
     */
    void ImportTeam1();

    /**
     * @brief Imports the away team player roster from a CSV file.
     *
     * Opens a file browser dialog to select a CSV file. Reads the file with UTF-8 encoding
     * (supporting German umlauts ä, ö, ü) and parses player data. Supports dynamic delimiter
     * detection: semicolon (;) or comma (,). Each line must have format: "[Number];[Name]".
     * Parses and adds each player to team2, then updates the team list display.
     * On file error, displays a warning message.
     *
     * @note CSV delimiter: Semicolon (;) preferred; comma (,) as fallback.
     * @note UTF-8 encoding: Preserves German umlauts and accents.
     * @note Input validation: Number must be integer; name must not be empty.
     * @note File extension: .csv or .CSV
     *
     * @see ImportTeam1() for home team equivalent
     * @see updateTeamList2() for display refresh
     */
    void ImportTeam2();

    /**
     * @brief Applies the operator's selected match state with validation and synchronization.
     *
     * Reads the selected radio button state (PreGame, FirstHalf, HalfTime, SecondHalf, PostGame)
     * and validates the transition. If no change is detected, returns immediately.
     *
     * If transitioning away from an active phase (FirstHalf/SecondHalf) while the timer is running,
     * displays a confirmation dialog. User may cancel (restores radio button) or confirm (stops/resets timer).
     *
     * On successful state change:
     * - Updates m_currentState
     * - Enables/disables Start Timer button based on phase (only enabled during FirstHalf/SecondHalf)
     * - Emits matchStateChanged() signal with new state integer value
     * - Notifies timer of phase change via timer::setPhase()
     *
     * @note State machine: PreGame → FirstHalf → HalfTime → SecondHalf → PostGame
     * @note Timer safety: Prevents accidental timer state loss during phase transitions.
     * @note Start Timer button: Automatically managed per phase; no manual manipulation needed.
     *
     * @see MatchState enum
     * @see handleTimerTimeout() for automatic half-time progression
     * @see timer::setPhase() for timer phase synchronization
     */
    void applyStateSelection();

    /**
     * @brief Handles timer timeout at end of first half (45 minutes).
     *
     * Called automatically by the timer at the 45-minute mark during FirstHalf phase.
     * Re-enables the Start Timer button and displays a confirmation dialog asking if the
     * operator wants to begin the second half.
     *
     * If confirmed (Yes):
     * - Sets radio button to SecondHalf
     * - Calls applyStateSelection() to transition state
     * - Calls StartTime() to begin second half timer
     *
     * If declined (No):
     * - Leaves timer stopped and waits for operator action
     * - Logs debug message "Second half not started."
     *
     * @note Connected to timer::timeout signal in setTimer().
     * @note Operates only during FirstHalf phase; ignores timeout in other phases.
     * @note User interaction: Modal dialog with Yes/No buttons.
     *
     * @see applyStateSelection() for state transition
     * @see StartTime() for timer resumption
     * @see timer::timeout signal
     */
    void handleTimerTimeout();

private:
    // Current state (default: PreGame)
    MatchState m_currentState = MatchState::PreGame;

    // Elements for connect with other classes
    home_team team1;
    away_team team2;

    timer *gametime;
    score_memory *ScoreMemory;
    QWidget *scoreboard = nullptr;

    // GUI elements for player management
    QPushButton *btnAddPlayerTeam1;
    QPushButton *btnAddPlayerTeam2;

    // GUI elements for goal management
    QPushButton *btnGoalTeam1;
    QPushButton *btnGoalTeam2;

    // GUI elements for timer-start and log buttons
    QPushButton *btnStartTimer;
    QPushButton *btnLog;

    // GUI elements for match state selection (state machine)
    QRadioButton *radioPreGame;
    QRadioButton *radioFirstHalf;
    QRadioButton *radioHalfTime;
    QRadioButton *radioSecondHalf;
    QRadioButton *radioPostGame;
    QPushButton *btnApplyState;

    QListWidget *listTeam1;
    QListWidget *listTeam2;

    // Time and score for the operator
    QLabel *score;
    QLabel *time;

    // Emblem features
    QPushButton *btnAddEmblemTeam1;
    QPushButton *btnAddEmblemTeam2;
    QString emblemTeam1;
    QString emblemTeam2;

    // Import player list buttons
    QPushButton *btnImportTeam1;
    QPushButton *btnImportTeam2;

    /**
     * @brief Updates the home team player list display.
     *
     * Clears and repopulates the home team list widget with current players from team1.
     * Each player is displayed as "[Number] - [Name]".
     *
     * @see updateTeamList2() for away team equivalent
     */
    void updateTeamList1();

    /**
     * @brief Updates the away team player list display.
     *
     * Clears and repopulates the away team list widget with current players from team2.
     * Each player is displayed as "[Number] - [Name]".
     *
     * @see updateTeamList1() for home team equivalent
     */
    void updateTeamList2();
};

#endif // CONTROLL_WINDOW_H

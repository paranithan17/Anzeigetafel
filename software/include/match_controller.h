/**
 * @file match_controller.h
 * @brief Central controller for match logic and state management.
 *
 * The match_controller is the business logic layer (Model in the Model-View-Controller pattern) that manages
 * all match-related operations including team rosters, scoring, timer control, and state
 * transitions. All user interactions from the GUI (controll_window) or web interface are
 * delegated to this controller, which processes them and emits signals for view updates.
 *
 * Features:
 * - Team roster management (add/remove players, CSV import)
 * - Goal logging with validation and emblem management
 * - Match timer control (start, stop, restart) with phase transitions
 * - Match state machine (PreGame → FirstHalf → HalfTime → SecondHalf → PostGame)
 * - Data persistence through score_memory and timer components
 * - Signal-based communication for Model-View-Controller pattern
 *
 * Design Pattern:
 * Implements the Model-View-Controller pattern where controll_window and future web interface
 * are views that delegate all business logic to this controller. This enables code reuse
 * across multiple UI implementations.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026 (Refactored for Model-View-Controller pattern)
 */
#ifndef MATCH_CONTROLLER_H
#define MATCH_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QElapsedTimer>
#include <QTimer>
#include <vector>
#include <memory>

#include "team.h"
#include "score_memory.h"
#include "timer.h"

class match_controller : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Identifies which team an operation targets.
     */
    enum class TeamSide
    {
        /** @brief Home team. */
        Home = 0,
        /** @brief Away team. */
        Away
    };

    /**
     * @brief Match phase state machine used by controller and views.
     */
    enum class MatchState
    {
        /** @brief Before kickoff, setup and presentation phase. */
        PreGame = 0,
        /** @brief First half is running. */
        FirstHalf,
        /** @brief Halftime break between first and second half. */
        HalfTime,
        /** @brief Second half is running. */
        SecondHalf,
        /** @brief Match has ended. */
        PostGame
    };

    /**
     * @brief Constructs the central match controller.
     *
     * Initializes controller state and binds model components used for
     * score persistence and timer operations.
     *
     * @param scoreMemory Pointer to score data storage/logic component
     * @param gameTimer Pointer to match timer component
     * @param parent Optional Qt parent object (default nullptr)
     * @return void
     */
    match_controller(score_memory *scoreMemory, timer *gameTimer, QObject *parent = nullptr);

    /**
     * @brief Returns the current match state.
     *
     * @return Current MatchState value
     */
    MatchState currentState() const;

    /**
     * @brief Returns read-only list of home team players.
     *
     * @return Const reference to home team player container
     */
    const std::vector<std::shared_ptr<player>> &getHomePlayers() const;

    /**
     * @brief Returns read-only list of away team players.
     *
     * @return Const reference to away team player container
     */
    const std::vector<std::shared_ptr<player>> &getAwayPlayers() const;

    /**
     * @brief Returns current home team score.
     *
     * @return Home goals count
     */
    int getHomeScore() const;

    /**
     * @brief Returns current away team score.
     *
     * @return Away goals count
     */
    int getAwayScore() const;

    /**
     * @brief Returns current formatted elapsed time string (MM:SS) for UI.
     *
     * @return Formatted time string or empty if unavailable
     */
    QString getCurrentTime() const;

    /**
     * @brief Returns current internally managed wall-clock time in HH:mm:ss.
     */
    QString getCurrentWallClockText() const;

    /**
     * @brief Returns current internally managed wall-clock as Unix epoch milliseconds.
     */
    qint64 getCurrentWallClockEpochMs() const;

    /**
     * @brief Synchronizes internal wall-clock from epoch milliseconds.
     */
    bool synchronizeWallClock(qint64 epochMs);

    /**
     * @brief Synchronizes internal wall-clock from ISO datetime string.
     */
    bool synchronizeWallClock(const QString &isoDateTime);

    /**
     * @brief Adds a player to the selected team roster.
     *
     * @param side Team side (Home/Away)
     * @param number Jersey number of the new player
     * @param name Display name of the new player
     * @return void
     */
    void addPlayer(TeamSide side, unsigned number, const QString &name);

    /**
     * @brief Removes a player from the selected team roster.
     *
     * @param side Team side (Home/Away)
     * @param number Jersey number of the player to remove
     * @return void
     */
    void removePlayer(TeamSide side, unsigned number);

    /**
     * @brief Imports a team roster from a CSV file.
     *
     * Parses player rows and appends valid entries to the target team.
     *
     * @param side Team side receiving imported players
     * @param filePath Absolute or relative path to CSV file
     * @param importedCount Optional output pointer for number of imported players
     * @return true if import completed successfully, otherwise false
     */
    bool importPlayersFromCsv(TeamSide side, const QString &filePath, int *importedCount = nullptr);

    // Goal management with validation from UI
    /**
     * @brief Input payload for adding a goal through controller validation.
     */
    struct GoalData
    {
        /** @brief Team credited with the goal event. */
        TeamSide scoringTeam;
        /** @brief Scorer jersey number from selected roster. */
        int playerNumber;
        /** @brief Scorer display name. */
        QString playerName;
        /** @brief Match minute to register for this goal. */
        unsigned goalMinute;
        /** @brief True if event is an own goal. */
        bool isOwnGoal;
    };

    /**
     * @brief Adds a goal after applying controller-side validation rules.
     *
     * Validates team/player consistency and own-goal behavior before committing.
     *
     * @param goalData Structured goal payload from UI
     * @return true if goal was accepted and stored, otherwise false
     */
    bool addGoalWithValidation(const GoalData &goalData);

    /**
     * @brief Removes the last registered goal.
     *
     * Updates score and goal history accordingly.
     *
     * @return void
     */
    void removeLastGoal();

    // Timer control operations delegated from UI
    /**
     * @brief Handles UI request to start timer based on current state.
     * @return void
     */
    void requestTimerStart();

    /**
     * @brief Handles UI request to restart timer for current phase.
     * @return void
     */
    void requestTimerRestart();

    /**
     * @brief Handles UI request to stop/pause timer.
     * @return void
     */
    void requestTimerStop();

    /**
     * @brief Handles UI request to reset match data and state.
     * @return void
     */
    void requestMatchReset();

    /**
     * @brief Resets score and timer without changing match state.
     *
     * Clears all goals and restarts the timer for current phase.
     *
     * @return void
     */
    void resetScoreAndTimer();

    // Match state transitions
    /**
     * @brief Suggests current goal minute based on timer state.
     *
     * Useful for pre-filling goal dialogs with a reasonable default minute.
     *
     * @return Suggested match minute
     */
    unsigned suggestedGoalMinute() const;

    /**
     * @brief Requests transition to a new match state.
     *
     * Applies transition rules, including handling active timer constraints.
     *
     * @param newState Target match state
     * @param confirmedRunningTimerStop Whether user confirmed stopping running timer
     * @return true if transition was applied, otherwise false
     */
    bool requestStateChange(MatchState newState, bool confirmedRunningTimerStop);

    /**
     * @brief Starts timer if current state allows active timing.
     *
     * @return true if timer was started, otherwise false
     */
    bool startTimer();

    // Emblem management
    /**
     * @brief Sets emblem path for selected team.
     *
     * Emits emblemChanged so views can refresh displayed logos.
     *
     * @param side Team side whose emblem should be updated
     * @param emblePath Image file path for the emblem
     * @return void
     */
    void setTeamEmblem(TeamSide side, const QString &emblePath);

    /**
     * @brief Returns emblem path for selected team.
     *
     * @param side Team side (Home/Away)
     * @return Emblem image file path for the selected team
     */
    QString getTeamEmblem(TeamSide side) const;

signals:
    /**
     * @brief Emitted when home roster changes.
     * @return void
     */
    void homePlayersChanged();

    /**
     * @brief Emitted when away roster changes.
     * @return void
     */
    void awayPlayersChanged();

    /**
     * @brief Emitted when score values change.
     * @return void
     */
    void scoreChanged();

    /**
     * @brief Emitted when timer display text changes.
     *
     * @param elapsedTime Formatted elapsed match time (e.g., MM:SS)
     * @return void
     */
    void timeChanged(const QString &elapsedTime);

    /**
     * @brief Emitted when match state changes.
     *
     * @param state Integer representation of MatchState enum
     * @return void
     */
    void matchStateChanged(int state);

    /**
     * @brief Emitted when UI should enable/disable timer start action.
     *
     * @param enabled True if starting timer is currently allowed
     * @return void
     */
    void startTimerEnabledChanged(bool enabled);

    /**
     * @brief Emitted when second-half start requires user decision.
     * @return void
     */
    void secondHalfDecisionNeeded();

    /**
     * @brief Emitted when a team emblem has changed.
     *
     * @param team Team identifier string used by views (e.g., "Home"/"Away")
     * @param filepath Updated emblem file path
     * @return void
     */
    void emblemChanged(const QString &team, const QString &filepath);

    /**
     * @brief Emitted whenever the internally managed wall-clock changes.
     *
     * @param displayTime Human readable wall-clock time (HH:mm:ss)
     * @param epochMs Unix epoch milliseconds of internal wall-clock
     */
    void wallClockUpdated(const QString &displayTime, qint64 epochMs);

public slots:
    /**
     * @brief Processes user decision for starting second half.
     *
     * @param startSecondHalf True to continue into second half, false otherwise
     * @return void
     */
    void onSecondHalfDecision(bool startSecondHalf);

private slots:
    /**
     * @brief Handles score-memory goal updates and emits view refresh signals.
     * @return void
     */
    void onGoalsUpdated();

    /**
     * @brief Handles timer text updates from timer component.
     *
     * @param elapsedTime Formatted elapsed match time
     * @return void
     */
    void onTimeUpdated(const QString &elapsedTime);

    /**
     * @brief Handles timer timeout/end-of-phase logic.
     * @return void
     */
    void onTimerTimeout();

    /**
     * @brief Emits periodic wall-clock updates while app is running.
     */
    void onWallClockTick();

private:
    /**
     * @brief Applies timer phase configuration for a given match state.
     *
     * Keeps timer behavior synchronized with state transitions.
     *
     * @param state Match state that defines timer phase setup
     * @return void
     */
    void applyTimerPhaseForState(MatchState state);

    /**
     * @brief Checks if state is an active playing phase.
     *
     * @param state State to evaluate
     * @return true for FirstHalf/SecondHalf, otherwise false
     */
    bool isActiveState(MatchState state) const;

    /**
     * @brief Returns current internally managed wall-clock in UTC.
     */
    QDateTime currentWallClockUtc() const;

    /** @brief Home team roster/model instance. */
    team m_homeTeam;
    /** @brief Away team roster/model instance. */
    team m_awayTeam;

    /** @brief Score memory/data component used by controller. */
    score_memory *m_scoreMemory = nullptr;
    /** @brief Timer component used by controller. */
    timer *m_gameTimer = nullptr;
    /** @brief Current controller match state. */
    MatchState m_state = MatchState::PreGame;

    /** @brief Stored emblem path for home team. */
    QString m_homeTeamEmblem;
    /** @brief Stored emblem path for away team. */
    QString m_awayTeamEmblem;

    /** @brief UTC base timestamp used for internal wall-clock progression. */
    QDateTime m_wallClockBaseUtc;
    /** @brief Monotonic elapsed timer since last clock synchronization. */
    QElapsedTimer m_wallClockElapsed;
    /** @brief Periodic notifier for wall-clock updates. */
    QTimer m_wallClockTickTimer;
};

#endif // MATCH_CONTROLLER_H

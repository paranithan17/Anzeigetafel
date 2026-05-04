/**
 * @file timer.h
 * @brief Match timer with phase-aware time tracking.
 *
 * Manages elapsed time for different game phases (PreGame, FirstHalf, HalfTime,
 * SecondHalf, PostGame). Tracks elapsed time independently for each half and emits
 * updates every second. Automatically stops at 45-minute mark for halves or based
 * on phase-specific timing.
 *
 * Features:
 * - Phase-aware time tracking for match states
 * - Independent timing for FirstHalf and SecondHalf
 * - Automatic timeout emission at phase end
 * - Per-second time update signals for display
 * - Play/pause capability with restart functionality
 * - Total elapsed time and phase-specific time retrieval
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 2026-04-13
 */

#ifndef TIMER_H
#define TIMER_H

#include "qobjectdefs.h"
#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QString>
#include "QDebug"

class timer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Game phase enumeration for state machine integration.
     *
     * Defines match phases to control timer behavior.
     * @see controll_window::MatchState
     */
    enum class GamePhase
    {
        /** @brief Before kickoff, no active match timing. */
        PreGame = 0,
        /** @brief First-half active timing window. */
        FirstHalf,
        /** @brief Halftime break phase. */
        HalfTime,
        /** @brief Second-half active timing window. */
        SecondHalf,
        /** @brief Match finished phase. */
        PostGame
    };

    /**
     * @brief Constructs match timer.
     *
     * @param parent Parent QObject (default nullptr)
     * @return void
     */
    timer(QObject *parent = nullptr);

    /**
     * @brief Starts timer for current phase.
     *
     * Only starts during FirstHalf or SecondHalf phases.
     * Emits timeUpdated signal every second.
     *
     * @return void
     */
    void start();

    /**
     * @brief Stops timer and pauses time tracking.
     * @return void
     */
    void stop();

    /**
     * @brief Restarts elapsed time counter to zero.
     *
     * Resets internal QElapsedTimer without changing phase.
     *
     * @return void
     */
    void restart();

    /**
     * @brief Resets timer to phase baseline and emits baseline time.
     *
     * Stops timing and emits "00:00" for FirstHalf or "45:00" for SecondHalf.
     *
     * @return void
     */
    void resetToPhaseStart();

    /**
     * @brief Checks if timer is currently running.
     *
     * @return true if timer is active, false otherwise
     */
    bool isRunning() const;

    /**
     * @brief Sets current game phase.
     *
     * Does not auto-start/stop timer; caller controls timing.
     *
     * @param phase New game phase
     * @return void
     */
    void setPhase(GamePhase phase);

    /**
     * @brief Gets current game phase.
     *
     * @return Current GamePhase
     */
    GamePhase phase() const { return m_phase; }

    /**
     * @brief Formats elapsed time for first half (00:00 - 45:00).
     *
     * @return Formatted time string
     */
    QString firsthalf();

    /**
     * @brief Formats elapsed time for second half (45:00 - 90:00).
     *
     * @return Formatted time string
     */
    QString secondhalf();

signals:
    /**
     * @brief Emitted when 45 minutes elapsed.
     * @return void
     */
    void timeout();

    /**
     * @brief Emitted every second with formatted time.
     *
     * @param elapsedTime Time string in MM:SS format
     * @return void
     */
    void timeUpdated(const QString &elapsedTime);

private slots:
    /**
     * @brief Updates time display based on current phase.
     *
     * Emits timeUpdated signal and checks for 45-minute timeout.
     *
     * @return void
     */
    void updateElapsedTime();

private:
    /** @brief High-resolution elapsed timer for active phase timing. */
    QElapsedTimer gameTime;
    /** @brief Internal 1-second ticker driving display updates. */
    QTimer updateTimer;
    /** @brief True while timer is actively running. */
    bool m_running;
    /** @brief Current phase controlling timing behavior. */
    GamePhase m_phase = GamePhase::PreGame;
};

#endif // TIMER_H

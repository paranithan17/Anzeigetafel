/**
 * @file timer.h
 * @brief Match timer with phase-aware time tracking.
 *
 * Manages match time for different game phases (PreGame, FirstHalf, HalfTime,
 * SecondHalf, PostGame). Tracks elapsed time during active phases and emits
 * updates every second. Automatically stops at 45-minute mark.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-07
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
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };

    /**
     * @brief Constructs match timer.
     *
     * @param parent Parent QObject (default nullptr)
     */
    timer(QObject *parent = nullptr);

    /**
     * @brief Starts timer for current phase.
     *
     * Only starts during FirstHalf or SecondHalf phases.
     * Emits timeUpdated signal every second.
     */
    void start();

    /**
     * @brief Stops timer and pauses time tracking.
     */
    void stop();

    /**
     * @brief Restarts elapsed time counter to zero.
     *
     * Resets internal QElapsedTimer without changing phase.
     */
    void restart();

    /**
     * @brief Resets timer to phase baseline and emits baseline time.
     *
     * Stops timing and emits "00:00" for FirstHalf or "45:00" for SecondHalf.
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
     */
    void timeout();

    /**
     * @brief Emitted every second with formatted time.
     *
     * @param elapsedTime Time string in MM:SS format
     */
    void timeUpdated(const QString &elapsedTime);

private slots:
    /**
     * @brief Updates time display based on current phase.
     *
     * Emits timeUpdated signal and checks for 45-minute timeout.
     */
    void updateElapsedTime();

private:
    QElapsedTimer gameTime;
    QTimer updateTimer;
    bool m_running;
    GamePhase m_phase = GamePhase::PreGame;
};

#endif // TIMER_H

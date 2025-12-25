/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard

 *
 * Class timer,
 * Initialzes the time for the game.
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 * @version v2.0, 07.06.2025 - There is a period handler implemented,
 *  which switches between 1st and 2nd half of a football game.
 *
 * @note This code has been created with help of chatgpt
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
    // Explicit game phases to integrate with the state machine
    enum class GamePhase
    {
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };

private:
    QElapsedTimer gameTime;
    QTimer updateTimer;
    bool m_running;
    GamePhase m_phase = GamePhase::PreGame;

public:
    timer(QObject *parent = nullptr);
    void start();
    void stop();          // stop time after 45 Min.
    QString firsthalf();  // "00:00"$
    QString secondhalf(); // restarts the timer for the 2nd half
    void restart();       // restarts the timer when the restart button is pushed.
    void resetToPhaseStart(); // stops and emits baseline time for current phase
    bool isRunning() const;

    // Phase control
    void setPhase(GamePhase phase);
    GamePhase phase() const { return m_phase; }

signals:
    void timeout();                               // Emitted when 45 Minutes have elapsed
    void timeUpdated(const QString &elapsedTime); // Emitted every second with update

private slots:
    void updateElapsedTime(); /** @note The function is unknown yet, since it was given by chatgpt */
};

#endif // TIMER_H

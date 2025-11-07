/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file timer.cpp
 * @class timer,
 * @brief
 * Initialzes the time for the game.
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 * @version v2.0, 07.06.2025 - There is a period handler implemented,
 *  which switches between 1st and 2nd half of a football game.
 *
 * @brief there is only two periodes. A first half 00:00 to 45:00 min
 *  and a secondhalf from 45:00 to 90:00 min. After every new start of the timer,
 *  the state will toggled.
 *
 * @note This code has been created with help of chatgpt
 *
 */

#include "timer.h"
#include "QDebug"
#include "QString"


timer::timer(QObject *parent)
    : QObject(parent),
    m_running(false),
    runningPeriod(false)
{
    connect(&updateTimer, &QTimer::timeout, this, &timer::updateElapsedTime);

}

void timer::start()
{
    /**
 * @brief This methods starts the QTimer with a interval of 1 second
 * and sets the timer state and the period.
 */
    if(!m_running){
        gameTime.start();
        updateTimer.start(1000); // setting interval for timer
        m_running = true;
        runningPeriod = !runningPeriod;
    }
    qDebug() << "Period:" << runningPeriod;
}

void timer::stop()
{
    /**
     * @brief Stops the QTimer and sets the timerstate to false
     */
    if (m_running) {

        updateTimer.stop();
        m_running = false;
    }
}

void timer::restart()
{
    /**
     * @brief Restarts the QTimer from 00:00. Sets the Period state back to 1st half.
     */
    gameTime.restart();
    runningPeriod = true;
}

bool timer::isRunning() const
{
    /**
     * @brief The global state of the timer.
     */

    return m_running;
};



QString timer::firsthalf()
{
    /**
     * @brief Gets the elapsed time from Qtimer and returns the time in 00:00 - 45:00
     * @note This part of the code was coded by chatgpt!
     */
    qint64 elapsedMs = gameTime.elapsed();
    int minutes = static_cast<int>(elapsedMs / 60000);
    int seconds = static_cast<int>((elapsedMs % 60000) / 1000);
    return QString ("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString timer::secondhalf()
{    /**
     * @brief Gets the elapsed time from Qtimer and returns the time in 45:00 - 90:00
     * @note This part of the code was coded by chatgpt!
     */
    qint64 elapsedMs = gameTime.elapsed();
    int minutes = static_cast<int>(elapsedMs / 60000) + 45;
    int seconds = static_cast<int>((elapsedMs % 60000) / 1000);
    return QString ("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}



void timer::updateElapsedTime(){
    /**
     * @brief Depending on which period is on going, this method emits the right timeformat to
     * the controll_window. When 45 minutes are reached, the stop method will called.
     */
    if(runningPeriod){
        emit timeUpdated(firsthalf());
    }
    else{
        emit timeUpdated(secondhalf());
    }
    if (gameTime.elapsed() >=  45 * 60 * 1000){ //45 Minutes
        stop();
        emit timeout();
    }
}

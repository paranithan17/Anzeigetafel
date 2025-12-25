/**
 * @file timer.cpp
 * @brief Implementation of match timer.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-07
 */

#include "timer.h"
#include "QDebug"
#include "QString"
#include <algorithm>

timer::timer(QObject *parent)
    : QObject(parent),
      m_running(false)
{
    connect(&updateTimer, &QTimer::timeout, this, &timer::updateElapsedTime);
}

void timer::start()
{
    if (!m_running && (m_phase == GamePhase::FirstHalf || m_phase == GamePhase::SecondHalf))
    {
        gameTime.start();
        updateTimer.start(1000);
        m_running = true;
        qDebug() << "Timer started for phase:" << static_cast<int>(m_phase);
    }
}

void timer::stop()
{
    if (m_running)
    {
        updateTimer.stop();
        m_running = false;
    }
}

void timer::restart()
{
    gameTime.restart();
}

void timer::resetToPhaseStart()
{
    updateTimer.stop();
    m_running = false;

    gameTime.restart();

    QString baseline;
    if (m_phase == GamePhase::SecondHalf)
    {
        baseline = QStringLiteral("45:00");
    }
    else
    {
        baseline = QStringLiteral("00:00");
    }

    emit timeUpdated(baseline);
}

bool timer::isRunning() const
{
    return m_running;
};

QString timer::firsthalf()
{
    qint64 elapsedMs = gameTime.elapsed();
    const qint64 maxMs = 45LL * 60 * 1000; // 45 minutes in ms
    elapsedMs = std::min(elapsedMs, maxMs);
    int minutes = static_cast<int>(elapsedMs / 60000);
    int seconds = static_cast<int>((elapsedMs % 60000) / 1000);
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString timer::secondhalf()
{
    qint64 elapsedMs = gameTime.elapsed();
    const qint64 maxMs = 45LL * 60 * 1000;
    elapsedMs = std::min(elapsedMs, maxMs);
    int minutes = static_cast<int>(elapsedMs / 60000) + 45;
    int seconds = static_cast<int>((elapsedMs % 60000) / 1000);
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void timer::updateElapsedTime()
{
    if (m_phase == GamePhase::FirstHalf)
    {
        emit timeUpdated(firsthalf());
    }
    else if (m_phase == GamePhase::SecondHalf)
    {
        emit timeUpdated(secondhalf());
    }
    else
    {
        return;
    }
    if (gameTime.elapsed() >= 45 * 60 * 1000)
    {
        stop();
        emit timeout();
    }
}

void timer::setPhase(GamePhase phase)
{
    m_phase = phase;
    qDebug() << "Timer phase set to:" << static_cast<int>(m_phase);
}

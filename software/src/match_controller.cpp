#/**
 * @file match_controller.cpp
 * @brief Match controller implementation.
 *
 * Coordinates match state, timer and scoring logic. This component mediates
 * between the `score_memory`, `timer`, team/player models and emits signals
 * consumed by the UI and WebSocket control interface. It also provides CSV
 * import for player lists and validates/records goal events.
 *
 * Author: Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */

#include "match_controller.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTimeZone>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

match_controller::match_controller(score_memory *scoreMemory, timer *gameTimer, QObject *parent)
    : QObject(parent),
      m_scoreMemory(scoreMemory),
      m_gameTimer(gameTimer)
{
    if (m_scoreMemory)
    {
        connect(m_scoreMemory, &score_memory::goalsUpdated, this, &match_controller::onGoalsUpdated);
    }

    if (m_gameTimer)
    {
        connect(m_gameTimer, &timer::timeUpdated, this, &match_controller::onTimeUpdated);
        connect(m_gameTimer, &timer::timeout, this, &match_controller::onTimerTimeout);
        applyTimerPhaseForState(m_state);
    }

    m_wallClockBaseUtc = QDateTime::currentDateTimeUtc();
    m_wallClockElapsed.start();
    m_wallClockTickTimer.setInterval(1000);
    connect(&m_wallClockTickTimer, &QTimer::timeout, this, &match_controller::onWallClockTick);
    m_wallClockTickTimer.start();
}

match_controller::MatchState match_controller::currentState() const
{
    return m_state;
}

const std::vector<std::shared_ptr<player>> &match_controller::getHomePlayers() const
{
    return m_homeTeam.getPlayers();
}

const std::vector<std::shared_ptr<player>> &match_controller::getAwayPlayers() const
{
    return m_awayTeam.getPlayers();
}

int match_controller::getHomeScore() const
{
    if (!m_scoreMemory)
    {
        return 0;
    }
    return m_scoreMemory->getHomeScore();
}

int match_controller::getAwayScore() const
{
    if (!m_scoreMemory)
    {
        return 0;
    }
    return m_scoreMemory->getAwayScore();
}

void match_controller::addPlayer(TeamSide side, unsigned number, const QString &name)
{
    if (side == TeamSide::Home)
    {
        m_homeTeam.addPlayer(number, name);
        emit homePlayersChanged();
        return;
    }

    m_awayTeam.addPlayer(number, name);
    emit awayPlayersChanged();
}

void match_controller::removePlayer(TeamSide side, unsigned number)
{
    if (side == TeamSide::Home)
    {
        m_homeTeam.removePlayer(number);
        emit homePlayersChanged();
        return;
    }

    m_awayTeam.removePlayer(number);
    emit awayPlayersChanged();
}

bool match_controller::importPlayersFromCsv(TeamSide side, const QString &filePath, int *importedCount)
{
    if (importedCount)
    {
        *importedCount = 0;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    bool importedAny = false;
    while (!in.atEnd())
    {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
        {
            continue;
        }

        const QChar delimiter = line.contains(';') ? ';' : ',';
        const QStringList fields = line.split(delimiter);
        if (fields.size() < 2)
        {
            continue;
        }

        bool ok = false;
        const int number = fields[0].trimmed().toInt(&ok);
        const QString name = fields[1].trimmed();
        if (ok && !name.isEmpty())
        {
            addPlayer(side, static_cast<unsigned>(number), name);
            importedAny = true;
            if (importedCount)
            {
                ++(*importedCount);
            }
        }
    }

    return true;
}

bool match_controller::addGoalWithValidation(const GoalData &goalData)
{
    const bool hasPlayer = goalData.playerNumber >= 0 && !goalData.playerName.trimmed().isEmpty();
    const bool anonymousGoal = goalData.playerNumber < 0 && goalData.playerName.trimmed().isEmpty();

    // Validate goal data
    if (!hasPlayer && !anonymousGoal)
    {
        qDebug() << "Invalid goal data: player data is incomplete";
        return false;
    }

    // Convert minute to timestamp
    const QString timeStamp = QString::number(goalData.goalMinute);

    const QString team = goalData.scoringTeam == TeamSide::Home ? QStringLiteral("Home") : QStringLiteral("Away");

    if (!m_scoreMemory)
    {
        return false;
    }

    m_scoreMemory->addGoal(goalData.playerNumber, goalData.playerName, timeStamp, team, goalData.isOwnGoal);
    return true;
}

void match_controller::removeLastGoal()
{
    if (!m_scoreMemory)
    {
        return;
    }

    m_scoreMemory->removeLastGoal();
}

void match_controller::requestTimerStart()
{
    if (!m_gameTimer || m_gameTimer->isRunning())
    {
        qDebug() << "Timer is already running or not available";
        return;
    }

    if (!isActiveState(m_state))
    {
        qDebug() << "Timer can only start in First/Second Half";
        return;
    }

    m_gameTimer->start();
    emit startTimerEnabledChanged(false);
}

void match_controller::requestTimerRestart()
{
    if (!m_gameTimer)
    {
        return;
    }

    m_gameTimer->restart();
}

void match_controller::requestTimerStop()
{
    if (!m_gameTimer)
    {
        return;
    }

    m_gameTimer->stop();
}

void match_controller::requestMatchReset()
{
    if (m_gameTimer)
    {
        m_gameTimer->resetToPhaseStart();
    }

    if (m_scoreMemory)
    {
        m_scoreMemory->resetGame();
    }

    emit startTimerEnabledChanged(isActiveState(m_state));
}

void match_controller::resetScoreAndTimer()
{
    if (m_scoreMemory)
    {
        m_scoreMemory->resetGame();
    }

    if (m_gameTimer)
    {
        m_gameTimer->resetToPhaseStart();
    }

    emit startTimerEnabledChanged(isActiveState(m_state));
}

unsigned match_controller::suggestedGoalMinute() const
{
    if (!m_gameTimer)
    {
        return 0;
    }

    QString elapsed;
    if (m_gameTimer->phase() == timer::GamePhase::FirstHalf)
    {
        elapsed = m_gameTimer->firsthalf();
    }
    else if (m_gameTimer->phase() == timer::GamePhase::SecondHalf)
    {
        elapsed = m_gameTimer->secondhalf();
    }

    if (elapsed.isEmpty())
    {
        return 0;
    }

    return static_cast<unsigned>(elapsed.section(':', 0, 0).toInt() + 1);
}

QString match_controller::getCurrentTime() const
{
    if (!m_gameTimer)
        return QString();

    if (m_gameTimer->phase() == timer::GamePhase::FirstHalf)
    {
        return m_gameTimer->firsthalf();
    }
    else if (m_gameTimer->phase() == timer::GamePhase::SecondHalf)
    {
        return m_gameTimer->secondhalf();
    }

    return QString();
}

QString match_controller::getCurrentWallClockText() const
{
    return currentWallClockUtc().toLocalTime().toString(QStringLiteral("HH:mm:ss"));
}

qint64 match_controller::getCurrentWallClockEpochMs() const
{
    return currentWallClockUtc().toMSecsSinceEpoch();
}

bool match_controller::synchronizeWallClock(qint64 epochMs)
{
    const QDateTime syncedUtc = QDateTime::fromMSecsSinceEpoch(epochMs, QTimeZone::UTC);
    if (!syncedUtc.isValid())
    {
        return false;
    }

    m_wallClockBaseUtc = syncedUtc;
    m_wallClockElapsed.restart();
    emit wallClockUpdated(getCurrentWallClockText(), getCurrentWallClockEpochMs());
    return true;
}

bool match_controller::synchronizeWallClock(const QString &isoDateTime)
{
    const QString value = isoDateTime.trimmed();
    if (value.isEmpty())
    {
        return false;
    }

    QDateTime parsed = QDateTime::fromString(value, Qt::ISODateWithMs);
    if (!parsed.isValid())
    {
        parsed = QDateTime::fromString(value, Qt::ISODate);
    }
    if (!parsed.isValid())
    {
        return false;
    }

    if (parsed.timeSpec() != Qt::UTC)
    {
        parsed = parsed.toUTC();
    }

    m_wallClockBaseUtc = parsed;
    m_wallClockElapsed.restart();
    emit wallClockUpdated(getCurrentWallClockText(), getCurrentWallClockEpochMs());
    return true;
}

bool match_controller::requestStateChange(MatchState newState, bool confirmedRunningTimerStop)
{
    if (newState == m_state)
    {
        return true;
    }

    const bool leavingActiveState = isActiveState(m_state);
    const bool enteringInactiveState = !isActiveState(newState);

    if (m_gameTimer && m_gameTimer->isRunning() && leavingActiveState && enteringInactiveState)
    {
        if (!confirmedRunningTimerStop)
        {
            return false;
        }

        m_gameTimer->stop();
        m_gameTimer->restart();
    }

    m_state = newState;
    applyTimerPhaseForState(m_state);

    emit startTimerEnabledChanged(isActiveState(m_state));
    emit matchStateChanged(static_cast<int>(m_state));
    return true;
}

bool match_controller::startTimer()
{
    if (!m_gameTimer || m_gameTimer->isRunning())
    {
        return false;
    }

    if (!isActiveState(m_state))
    {
        qDebug() << "Timer can only start in First/Second Half";
        return false;
    }

    m_gameTimer->start();
    emit startTimerEnabledChanged(false);
    return true;
}

void match_controller::setTeamEmblem(TeamSide side, const QString &emblePath)
{
    if (side == TeamSide::Home)
    {
        m_homeTeamEmblem = emblePath;
        emit emblemChanged(QStringLiteral("Home"), emblePath);
    }
    else
    {
        m_awayTeamEmblem = emblePath;
        emit emblemChanged(QStringLiteral("Away"), emblePath);
    }
}

QString match_controller::getTeamEmblem(TeamSide side) const
{
    return side == TeamSide::Home ? m_homeTeamEmblem : m_awayTeamEmblem;
}

void match_controller::onSecondHalfDecision(bool startSecondHalf)
{
    if (!startSecondHalf)
    {
        qDebug() << "Second half not started.";
        return;
    }

    const bool changed = requestStateChange(MatchState::SecondHalf, true);
    if (changed)
    {
        startTimer();
    }
}

void match_controller::onGoalsUpdated()
{
    emit scoreChanged();
}

void match_controller::onTimeUpdated(const QString &elapsedTime)
{
    emit timeChanged(elapsedTime);
}

void match_controller::onTimerTimeout()
{
    emit startTimerEnabledChanged(true);

    if (m_gameTimer && m_gameTimer->phase() == timer::GamePhase::FirstHalf)
    {
        emit secondHalfDecisionNeeded();
    }
}

void match_controller::onWallClockTick()
{
    emit wallClockUpdated(getCurrentWallClockText(), getCurrentWallClockEpochMs());
}

QDateTime match_controller::currentWallClockUtc() const
{
    if (!m_wallClockElapsed.isValid())
    {
        return m_wallClockBaseUtc;
    }

    return m_wallClockBaseUtc.addMSecs(m_wallClockElapsed.elapsed());
}

void match_controller::applyTimerPhaseForState(MatchState state)
{
    if (!m_gameTimer)
    {
        return;
    }

    switch (state)
    {
    case MatchState::PreGame:
        m_gameTimer->setPhase(timer::GamePhase::PreGame);
        break;
    case MatchState::FirstHalf:
        m_gameTimer->setPhase(timer::GamePhase::FirstHalf);
        break;
    case MatchState::HalfTime:
        m_gameTimer->setPhase(timer::GamePhase::HalfTime);
        break;
    case MatchState::SecondHalf:
        m_gameTimer->setPhase(timer::GamePhase::SecondHalf);
        break;
    case MatchState::PostGame:
        m_gameTimer->setPhase(timer::GamePhase::PostGame);
        break;
    }
}

bool match_controller::isActiveState(MatchState state) const
{
    return state == MatchState::FirstHalf || state == MatchState::SecondHalf;
}

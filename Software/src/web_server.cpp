/**
 * @file web_server.cpp
 * @brief Implementation of Websocket server.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 2026-05-07
 */

#include "web_server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

web_server::web_server(match_controller *controller, QObject *parent)
    : QObject(parent),
      m_server(QStringLiteral("Anzeigetafel WebSocket Server"),
               QWebSocketServer::NonSecureMode,
               this),
      m_controller(controller)
{
    // New browser connection
    connect(&m_server,
            &QWebSocketServer::newConnection,
            this,
            &web_server::onNewConnection);

    // Match controller -> browser update
    if (m_controller)
    {
        connect(m_controller,
                &match_controller::matchStateChanged,
                this,
                &web_server::broadcastMatchState);

        connect(m_controller,
                &match_controller::homePlayersChanged,
                this,
                &web_server::broadcastHomePlayersList);

        connect(m_controller,
                &match_controller::awayPlayersChanged,
                this,
                &web_server::broadcastAwayPlayersList);
    }
}

// Start WebSocket server
bool web_server::start(quint16 port)
{
    const QHostAddress serverAddress(QStringLiteral("192.168.1.36"));
    const bool ok = m_server.listen(serverAddress, port);

    if (ok)
        qDebug() << "WebSocket server started on" << m_server.serverAddress().toString() << "port" << port;
    else
        qDebug() << "WebSocket server error:" << m_server.errorString();

    return ok;
}

// Accept new browser connection
void web_server::onNewConnection()
{
    QWebSocket *client = m_server.nextPendingConnection();

    if (!client)
        return;

    // Store connected browser
    m_clients.append(client);

    // Receive text messages from browser
    connect(client, &QWebSocket::textMessageReceived, this, &web_server::onTextMessageReceived);

    // Remove browser when disconnected
    connect(client,
            &QWebSocket::disconnected, this, &web_server::onClientDisconnected);

    qDebug() << "WebSocket client connected:" << client->peerAddress().toString();

    // Send current match state immediately after connection
    if (m_controller)
    {
        client->sendTextMessage(
            createMatchStateJson(static_cast<int>(m_controller->currentState())));

        // Send current player lists
        const auto homePlayers = m_controller->getHomePlayers();
        QJsonObject homeObj;
        homeObj["type"] = "playersList";
        homeObj["team"] = "Home";
        QJsonArray homeArray;
        for (const auto &p : homePlayers)
        {
            if (p)
            {
                QJsonObject playerObj;
                playerObj["number"] = static_cast<int>(p->getNumber());
                playerObj["name"] = p->getName();
                homeArray.append(playerObj);
            }
        }
        homeObj["players"] = homeArray;
        client->sendTextMessage(QString::fromUtf8(QJsonDocument(homeObj).toJson(QJsonDocument::Compact)));

        const auto awayPlayers = m_controller->getAwayPlayers();
        QJsonObject awayObj;
        awayObj["type"] = "playersList";
        awayObj["team"] = "Away";
        QJsonArray awayArray;
        for (const auto &p : awayPlayers)
        {
            if (p)
            {
                QJsonObject playerObj;
                playerObj["number"] = static_cast<int>(p->getNumber());
                playerObj["name"] = p->getName();
                awayArray.append(playerObj);
            }
        }
        awayObj["players"] = awayArray;
        client->sendTextMessage(QString::fromUtf8(QJsonDocument(awayObj).toJson(QJsonDocument::Compact)));
    }
}

// Receive JSON message from browser
void web_server::onTextMessageReceived(const QString &message)
{
    QJsonParseError parseError;

    // Convert received text into JSON document
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
    {
        qDebug() << "Invalid JSON from browser:" << message;
        return;
    }

    // Handle JSON command
    handleJsonCommand(doc.object());
}

// Remove disconnected browser
void web_server::onClientDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (!client)
        return;

    m_clients.removeAll(client);
    client->deleteLater();

    qDebug() << "WebSocket client disconnected";
}

// Handle browser command
void web_server::handleJsonCommand(const QJsonObject &obj)
{
    if (!m_controller)
        return;

    const QString type = obj.value("type").toString();

    // Browser wants to change match state
    if (type == "setMatchState")
    {
        const int stateValue = obj.value("state").toInt(-1);

        if (stateValue < 0 || stateValue > 4)
        {
            qDebug() << "Invalid match state:" << stateValue;
            return;
        }

        const auto newState =
            static_cast<match_controller::MatchState>(stateValue);

        // true = confirms state change even if timer must be stopped
        m_controller->requestStateChange(newState, true);
    }
    else if (type == "startTimer")
    {
        if (m_controller)
        {
            const bool started = m_controller->startTimer();
            qDebug() << "Start timer requested from browser, started=" << started;
        }
    }
    else if (type == "goal")
    {
        const QString team = obj.value("team").toString();
        qDebug() << "Goal request from browser for team:" << team;
        // TODO: Implement adding goal via match_controller with proper payload
    }
    else if (type == "addPlayer")
    {
        const QString team = obj.value("team").toString();
        const int number = obj.value("number").toInt(-1);
        const QString name = obj.value("name").toString();

        if (number >= 0 && !name.isEmpty())
        {
            const auto side = (team == "Home") ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
            m_controller->addPlayer(side, static_cast<unsigned>(number), name);
            qDebug() << "Added player:" << team << "#" << number << name;
        }
    }
    else if (type == "importPlayers")
    {
        const QString team = obj.value("team").toString();
        const QJsonArray playersArray = obj.value("players").toArray();

        // For browser import, build a temporary CSV and use importPlayersFromCsv
        // or directly add each player
        for (const QJsonValue &val : playersArray)
        {
            const QJsonObject playerObj = val.toObject();
            const int number = playerObj.value("number").toInt(-1);
            const QString name = playerObj.value("name").toString();

            if (number >= 0 && !name.isEmpty())
            {
                const auto side = (team == "Home") ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
                m_controller->addPlayer(side, static_cast<unsigned>(number), name);
            }
        }

        qDebug() << "Imported" << playersArray.size() << "players for team:" << team;
    }
    else if (type == "removePlayer")
    {
        const QString team = obj.value("team").toString();
        const int number = obj.value("number").toInt(-1);

        if (number >= 0)
        {
            const auto side = (team == "Home") ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
            m_controller->removePlayer(side, static_cast<unsigned>(number));
            qDebug() << "Removed player:" << team << "#" << number;
        }
    }
}

// Send match state to all connected browsers
void web_server::broadcastMatchState(int state)
{
    const QString json = createMatchStateJson(state);

    for (QWebSocket *client : std::as_const(m_clients))
    {
        if (client && client->isValid())
        {
            client->sendTextMessage(json);
        }
    }
}

// Broadcast home team players list to all browsers
void web_server::broadcastHomePlayersList()
{
    if (!m_controller)
        return;

    const auto players = m_controller->getHomePlayers();
    broadcastPlayersList("Home", players);
}

// Broadcast away team players list to all browsers
void web_server::broadcastAwayPlayersList()
{
    if (!m_controller)
        return;

    const auto players = m_controller->getAwayPlayers();
    broadcastPlayersList("Away", players);
}

// Create JSON message for browser
QString web_server::createMatchStateJson(int state) const
{
    QJsonObject obj;

    obj["type"] = "matchState";
    obj["state"] = state;

    switch (state)
    {
    case 0:
        obj["stateName"] = "PreGame";
        break;
    case 1:
        obj["stateName"] = "FirstHalf";
        break;
    case 2:
        obj["stateName"] = "HalfTime";
        break;
    case 3:
        obj["stateName"] = "SecondHalf";
        break;
    case 4:
        obj["stateName"] = "PostGame";
        break;
    default:
        obj["stateName"] = "Unknown";
        break;
    }

    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

// Broadcast player list to all browsers
void web_server::broadcastPlayersList(const QString &team, const std::vector<std::shared_ptr<player>> &players)
{
    QJsonObject obj;
    obj["type"] = "playersList";
    obj["team"] = team;

    QJsonArray playersArray;
    for (const auto &p : players)
    {
        if (p)
        {
            QJsonObject playerObj;
            playerObj["number"] = static_cast<int>(p->getNumber());
            playerObj["name"] = p->getName();
            playersArray.append(playerObj);
        }
    }

    obj["players"] = playersArray;

    const QString json = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    for (QWebSocket *client : std::as_const(m_clients))
    {
        if (client && client->isValid())
        {
            client->sendTextMessage(json);
        }
    }
}
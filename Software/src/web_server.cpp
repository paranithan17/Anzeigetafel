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
    }
}

// Start WebSocket server
bool web_server::start(quint16 port)
{
    const bool ok = m_server.listen(QHostAddress::AnyIPv4, port);

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
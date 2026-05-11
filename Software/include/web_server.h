/**
 * @file web_server.h
 * @brief WebSocket server for web-based match control.
 *
 * Provides a WebSocket server that allows web clients to control match state.
 * Currently supports only match state changes (PreGame, FirstHalf, HalfTime, PostGame).
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 2026-05-07
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QList>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

#include "match_controller.h"

/**
 * @class web_server
 * @brief WebSocket server for remote browser control.
 *
 * This class replaces the simple HTTP server approach with a persistent
 * WebSocket connection. The browser stays connected and can send JSON commands.
 */
class web_server : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the WebSocket server.
     *
     * @param controller Pointer to the central match controller
     * @param parent Optional Qt parent
     */
    explicit web_server(match_controller *controller, QObject *parent = nullptr);

    /**
     * @brief Starts the WebSocket server.
     *
     * @param port TCP port for WebSocket connection
     * @return true if server started successfully
     */
    bool start(quint16 port = 80);

private slots:
    /**
     * @brief Called when a browser opens a WebSocket connection.
     */
    void onNewConnection();

    /**
     * @brief Handles incoming JSON messages from browser.
     *
     * @param message Text message received from WebSocket client
     */
    void onTextMessageReceived(const QString &message);

    /**
     * @brief Removes disconnected browser client from client list.
     */
    void onClientDisconnected();

    /**
     * @brief Sends current match state to all connected browsers.
     *
     * @param state Current match state as integer
     */
    void broadcastMatchState(int state);

    /**
     * @brief Broadcasts home team players list to all browsers.
     */
    void broadcastHomePlayersList();

    /**
     * @brief Broadcasts away team players list to all browsers.
     */
    void broadcastAwayPlayersList();

private:
    QWebSocketServer m_server;      ///< WebSocket server instance
    QList<QWebSocket *> m_clients;  ///< Connected browser clients
    match_controller *m_controller; ///< Central match logic controller

    /**
     * @brief Handles a parsed JSON command from browser.
     *
     * @param obj JSON object received from client
     */
    void handleJsonCommand(const QJsonObject &obj);

    /**
     * @brief Converts controller state integer into JSON message.
     *
     * @param state Match state as integer
     * @return JSON text message
     */
    QString createMatchStateJson(int state) const;

    /**
     * @brief Broadcasts a team's player list to all connected browsers.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param players Vector of player objects
     */
    void broadcastPlayersList(const QString &team, const std::vector<std::shared_ptr<player>> &players);

    /**
     * @brief Sends all saved emblem images from the emblem folder to the requesting browser.
     *
     * Reads image files from the configured emblem directory, converts them to Base64
     * data URLs and sends them as a JSON list to the connected WebSocket client.
     */
    void sendSavedEmblems();

    /**
     * @brief Selects an already saved emblem image for a team.
     *
     * @param team Team identifier from browser: "Home" or "Away"
     * @param filePath Absolute path of the selected emblem image
     */
    void handleSelectSavedEmblem(const QString &team, const QString &filePath);

    /**
     * @brief Saves emblem upload from browser into directory /home/scorerboard/Anzeigetafel/Import, and forwards update to controller.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param fileName Name of the uploaded emblem file
     * @param dataUrl Base64-encoded image data
     */
    void handleSetEmblem(const QString &team, const QString &fileName, const QString &dataUrl);
};

#endif // WEB_SERVER_H
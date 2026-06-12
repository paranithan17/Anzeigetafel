/**
 * @file websocket.h
 * @brief WebSocket server for web-based match control.
 *
 * Provides a WebSocket server that allows web clients to control match state.
 * Currently supports only match state changes (PreGame, FirstHalf, HalfTime, PostGame).
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

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
#include <vector>
#include <memory>

#include "match_controller.h"

/**
 * @class websocket
 * @brief WebSocket server for remote browser control.
 *
 * This class replaces the simple HTTP server approach with a persistent
 * WebSocket connection. The browser stays connected and can send JSON commands.
 */
class websocket : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the WebSocket server.
     *
     * @param controller Pointer to the central match controller
     * @param parent Optional Qt parent
     */
    explicit websocket(match_controller *controller, QObject *parent = nullptr);

    /**
     * @brief Starts the WebSocket server.
     *
     * @param port TCP port for WebSocket connection
     * @return true if server started successfully
     */
    bool start_server(quint16 port = 8080);
    /**
     * @brief Compatibility wrapper for legacy `start()` calls.
     */
    bool start(quint16 port = 8080) { return start_server(port); }

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

    /*
     * Note: individual home/away slot methods were removed in favor of
     * a single reusable broadcaster `broadcastPlayerList` which accepts
     * the team identifier and the player vector.
     */

    /**
     * @brief Broadcasts combined score and time to all browsers.
     */
    void broadcastScoreTime();

    /**
     * @brief Broadcasts time update (slot signature matching controller).
     */
    void broadcastTimeUpdate(const QString &elapsedTime);

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
     * @brief Create JSON for a team's player list (used for single-client sends).
     *
     * @param team Team identifier ("Home" or "Away")
     * @param players Vector of player objects
     * @return JSON text message
     */
    QString createPlayersListJson(const QString &team, const std::vector<std::shared_ptr<player>> &players) const;

    /**
     * @brief Broadcasts a team's player list to all connected browsers.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param players Vector of player objects
     */
    void broadcastPlayerList(const QString &team, const std::vector<std::shared_ptr<player>> &players);

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

    /**
     * @brief Sends list of available CSV player files to the requesting browser.
     *
     * Reads CSV files from the configured directory and sends them as a JSON list
     * to the connected WebSocket client.
     */
    void sendSavedCsvFiles();

    /**
     * @brief Selects and imports players from a saved CSV file.
     *
     * @param team Team identifier from browser: "Home" or "Away"
     * @param filePath Absolute path of the CSV file to import
     */
    void handleSelectSavedCsv(const QString &team, const QString &filePath);

    /**
     * @brief Saves CSV file upload from browser and imports players.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param fileName Name of the uploaded CSV file
     * @param fileData Content of the CSV file
     */
    void handleSetCsvFile(const QString &team, const QString &fileName, const QString &fileData);

    /**
     * @brief Parses a CSV file and imports players for a team.
     *
     * @param team Team identifier ("Home" or "Away")
     * @param file Open QFile containing CSV data (number,name format)
     */
    void parseAndImportCsv(const QString &team, QFile &file);
};

#endif // WEBSOCKET_H
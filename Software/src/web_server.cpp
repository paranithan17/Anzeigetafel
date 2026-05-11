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
#include <QSet>
#include <QCoreApplication>
#include <QPixmap>
#include <QBuffer>

namespace
{
    QString resolveEmblemImportDir()
    {
        const QString preferred = QStringLiteral("/home/scorerboard/Anzeigetafel/Import");

        QDir preferredDir(preferred);
        if ((!preferredDir.exists() && preferredDir.mkpath(".")) || preferredDir.exists())
        {
            const QFileInfo preferredInfo(preferredDir.absolutePath());
            if (preferredInfo.exists() && preferredInfo.isWritable())
            {
                return preferredDir.absolutePath();
            }
        }

        QDir fallback(QCoreApplication::applicationDirPath());
        if (fallback.cdUp())
        {
            const QString candidate = fallback.absoluteFilePath(QStringLiteral("Import"));
            QDir candidateDir(candidate);
            if ((!candidateDir.exists() && candidateDir.mkpath(".")) || candidateDir.exists())
            {
                return candidateDir.absolutePath();
            }
        }

        return QDir::current().absoluteFilePath(QStringLiteral("Import"));
    }
}

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

        // Accept both new payload keys (playerNumber/playerName) and legacy keys (number/name)
        int playerNumber = obj.value("playerNumber").toInt(-1);
        if (playerNumber < 0)
        {
            playerNumber = obj.value("number").toInt(-1);
        }

        QString playerName = obj.value("playerName").toString();
        if (playerName.isEmpty())
        {
            playerName = obj.value("name").toString();
        }

        unsigned goalMinute = static_cast<unsigned>(obj.value("goalMinute").toInt(-1));
        if (goalMinute == static_cast<unsigned>(-1))
        {
            goalMinute = m_controller->suggestedGoalMinute();
        }

        const bool isOwnGoal = obj.value("isOwnGoal").toBool(false);

        if (team != "Home" && team != "Away")
        {
            qDebug() << "Goal request rejected: invalid team:" << team;
            return;
        }

        match_controller::GoalData goalData;
        goalData.scoringTeam = (team == "Home") ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
        goalData.playerNumber = playerNumber;
        goalData.playerName = playerName;
        goalData.goalMinute = goalMinute;
        goalData.isOwnGoal = isOwnGoal;

        const bool added = m_controller->addGoalWithValidation(goalData);
        qDebug() << "Goal request from browser:" << team << "#" << playerNumber << playerName
                 << "minute=" << goalMinute << "ownGoal=" << isOwnGoal << "accepted=" << added;
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
    else if (type == "getSavedEmblems")
    {
        sendSavedEmblems();
        return;
    }
    else if (type == "selectSavedEmblem")
    {
        const QString team = obj.value("team").toString();
        const QString filePath = obj.value("filePath").toString();

        handleSelectSavedEmblem(team, filePath);
        return;
    }
    else if (type == "importEmblem" || type == "setEmblem")
    {
        const QString team = obj.value("team").toString();
        const QString fileName = obj.value("fileName").toString();
        QString dataUrl = obj.value("dataUrl").toString();

        if (dataUrl.isEmpty())
        {
            dataUrl = obj.value("data").toString();
        }

        handleSetEmblem(team, fileName, dataUrl);
        return;
    }
    else if (type == "getSavedCsvFiles")
    {
        sendSavedCsvFiles();
        return;
    }
    else if (type == "selectSavedCsv")
    {
        const QString team = obj.value("team").toString();
        const QString filePath = obj.value("filePath").toString();

        handleSelectSavedCsv(team, filePath);
        return;
    }
    else if (type == "setCsvFile")
    {
        const QString team = obj.value("team").toString();
        const QString fileName = obj.value("fileName").toString();
        const QString fileData = obj.value("fileData").toString();

        handleSetCsvFile(team, fileName, fileData);
        return;
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

void web_server::sendSavedEmblems()
{
    QJsonArray emblemArray;

    const QString importDirPath = resolveEmblemImportDir();
    const QString legacyDirPath = QStringLiteral("/home/scorerboard/Anzeigetafel/Import");

    QSet<QString> seenPaths;
    const QStringList searchDirs = {importDirPath, legacyDirPath};

    for (const QString &dirPath : searchDirs)
    {
        QDir dir(dirPath);
        if (!dir.exists())
        {
            continue;
        }

        const QFileInfoList files = dir.entryInfoList(
            {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.webp"},
            QDir::Files | QDir::Readable,
            QDir::Name);

        for (const QFileInfo &fileInfo : files)
        {
            const QString absolutePath = fileInfo.absoluteFilePath();
            if (seenPaths.contains(absolutePath))
            {
                continue;
            }

            QFile file(absolutePath);

            if (!file.open(QIODevice::ReadOnly))
            {
                continue;
            }

            const QByteArray imageData = file.readAll();
            file.close();

            QString suffix = fileInfo.suffix().toLower();
            QString mimeType = "image/png";

            if (suffix == "jpg" || suffix == "jpeg")
                mimeType = "image/jpeg";
            else if (suffix == "bmp")
                mimeType = "image/bmp";
            else if (suffix == "webp")
                mimeType = "image/webp";

            // Scale down image to preview size (100x100) for faster loading
            QPixmap pixmap;
            pixmap.loadFromData(imageData);

            QByteArray previewData;
            if (!pixmap.isNull())
            {
                // Scale to fit 100x100 while maintaining aspect ratio
                QPixmap scaled = pixmap.scaledToWidth(100, Qt::SmoothTransformation);
                if (scaled.height() > 100)
                {
                    scaled = pixmap.scaledToHeight(100, Qt::SmoothTransformation);
                }

                // Encode scaled image to base64
                QBuffer buffer(&previewData);
                buffer.open(QIODevice::WriteOnly);
                scaled.save(&buffer, suffix == "jpg" || suffix == "jpeg" ? "JPEG" : suffix == "bmp" ? "BMP"
                                                                                : suffix == "webp"  ? "WEBP"
                                                                                                    : "PNG",
                            85);
                buffer.close();
            }
            else
            {
                previewData = imageData;
            }

            QString base64 =
                QString("data:%1;base64,%2")
                    .arg(mimeType)
                    .arg(QString::fromUtf8(previewData.toBase64()));

            QJsonObject emblemObj;
            emblemObj["fileName"] = fileInfo.fileName();
            emblemObj["filePath"] = absolutePath;
            emblemObj["data"] = base64;

            emblemArray.append(emblemObj);
            seenPaths.insert(absolutePath);
        }
    }

    QJsonObject response;
    response["type"] = "savedEmblemsList";
    response["emblems"] = emblemArray;

    QString json =
        QString::fromUtf8(QJsonDocument(response).toJson(QJsonDocument::Compact));

    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client && client->isValid())
    {
        client->sendTextMessage(json);
        return;
    }

    for (QWebSocket *connectedClient : std::as_const(m_clients))
    {
        if (connectedClient && connectedClient->isValid())
        {
            connectedClient->sendTextMessage(json);
        }
    }
}

void web_server::handleSelectSavedEmblem(const QString &team, const QString &filePath)
{
    if (!m_controller)
        return;

    if (team != "Home" && team != "Away")
    {
        qDebug() << "Select emblem rejected: invalid team" << team;
        return;
    }

    const QString normalizedPath = QDir::cleanPath(filePath);

    if (!QFileInfo::exists(normalizedPath) || !QFileInfo(normalizedPath).isFile())
    {
        qDebug() << "Select emblem rejected: file does not exist" << normalizedPath;
        return;
    }

    if (team == "Home")
    {
        m_controller->setTeamEmblem(match_controller::TeamSide::Home, normalizedPath);
    }
    else
    {
        m_controller->setTeamEmblem(match_controller::TeamSide::Away, normalizedPath);
    }

    qDebug() << "Selected saved emblem:" << team << normalizedPath;
}

/*
 * Handle emblem upload from browser, save to disk, and update controller
 */
void web_server::handleSetEmblem(const QString &team,
                                 const QString &fileName,
                                 const QString &dataUrl)
{
    if (!m_controller)
        return;

    QString base64Data = dataUrl;

    const int commaIndex = base64Data.indexOf(',');
    if (commaIndex >= 0)
    {
        base64Data = base64Data.mid(commaIndex + 1);
    }

    QByteArray imageData = QByteArray::fromBase64(base64Data.toUtf8());

    if (imageData.isEmpty())
    {
        qDebug() << "Emblem upload failed: empty image data";
        return;
    }

    if (team != "Home" && team != "Away")
    {
        qDebug() << "Emblem upload rejected: invalid team" << team;
        return;
    }

    QDir dir(resolveEmblemImportDir());

    if (!dir.exists() && !dir.mkpath("."))
    {
        qDebug() << "Could not create emblem directory:" << dir.absolutePath();
        return;
    }

    QString safeFileName = fileName.trimmed();
    if (safeFileName.isEmpty())
    {
        safeFileName = QStringLiteral("emblem.png");
    }
    safeFileName.replace(" ", "_");

    const QString fullPath = dir.absoluteFilePath(team + "_" + safeFileName);

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not save emblem:" << fullPath << "error:" << file.errorString();
        return;
    }

    const qint64 bytesWritten = file.write(imageData);
    file.close();

    if (bytesWritten <= 0)
    {
        qDebug() << "Could not write emblem data to:" << fullPath;
        return;
    }

    if (team == "Home")
    {
        m_controller->setTeamEmblem(match_controller::TeamSide::Home, fullPath);
    }
    else if (team == "Away")
    {
        m_controller->setTeamEmblem(match_controller::TeamSide::Away, fullPath);
    }

    qDebug() << "Emblem saved:" << fullPath;

    // Refresh emblem list in browser immediately after successful upload.
    sendSavedEmblems();
}

/*
 * Send list of available CSV files to browser
 */
void web_server::sendSavedCsvFiles()
{
    QJsonArray csvArray;

    const QString importDirPath = resolveEmblemImportDir();
    const QString legacyDirPath = QStringLiteral("/home/scorerboard/Anzeigetafel/Import");

    QSet<QString> seenPaths;
    const QStringList searchDirs = {importDirPath, legacyDirPath};

    for (const QString &dirPath : searchDirs)
    {
        QDir dir(dirPath);
        if (!dir.exists())
        {
            continue;
        }

        const QFileInfoList files = dir.entryInfoList(
            {"*.csv"},
            QDir::Files | QDir::Readable,
            QDir::Name);

        for (const QFileInfo &fileInfo : files)
        {
            const QString absolutePath = fileInfo.absoluteFilePath();
            if (seenPaths.contains(absolutePath))
            {
                continue;
            }

            QJsonObject csvObj;
            csvObj["fileName"] = fileInfo.fileName();
            csvObj["filePath"] = absolutePath;
            csvObj["size"] = static_cast<int>(fileInfo.size());

            csvArray.append(csvObj);
            seenPaths.insert(absolutePath);
        }
    }

    QJsonObject response;
    response["type"] = "savedCsvFilesList";
    response["files"] = csvArray;

    QString json =
        QString::fromUtf8(QJsonDocument(response).toJson(QJsonDocument::Compact));

    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client && client->isValid())
    {
        client->sendTextMessage(json);
        return;
    }

    for (QWebSocket *connectedClient : std::as_const(m_clients))
    {
        if (connectedClient && connectedClient->isValid())
        {
            connectedClient->sendTextMessage(json);
        }
    }
}

/*
 * Handle selection of a saved CSV file, parse it, and import players
 */
void web_server::handleSelectSavedCsv(const QString &team, const QString &filePath)
{
    if (!m_controller)
        return;

    if (team != "Home" && team != "Away")
    {
        qDebug() << "Select CSV rejected: invalid team" << team;
        return;
    }

    const QString normalizedPath = QDir::cleanPath(filePath);

    if (!QFileInfo::exists(normalizedPath) || !QFileInfo(normalizedPath).isFile())
    {
        qDebug() << "Select CSV rejected: file does not exist" << normalizedPath;
        return;
    }

    QFile file(normalizedPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Could not open CSV file:" << normalizedPath << "error:" << file.errorString();
        return;
    }

    parseAndImportCsv(team, file);
    file.close();

    qDebug() << "Imported players from saved CSV:" << team << normalizedPath;
}

/*
 * Handle CSV file upload from browser, save to disk, and import players
 */
void web_server::handleSetCsvFile(const QString &team,
                                  const QString &fileName,
                                  const QString &fileData)
{
    if (!m_controller)
        return;

    if (team != "Home" && team != "Away")
    {
        qDebug() << "CSV upload rejected: invalid team" << team;
        return;
    }

    QDir dir(resolveEmblemImportDir());

    if (!dir.exists() && !dir.mkpath("."))
    {
        qDebug() << "Could not create CSV directory:" << dir.absolutePath();
        return;
    }

    QString safeFileName = fileName.trimmed();
    if (safeFileName.isEmpty())
    {
        safeFileName = QStringLiteral("players.csv");
    }

    // Only keep the filename part (no path)
    safeFileName = QFileInfo(safeFileName).fileName();
    safeFileName.replace(" ", "_");

    const QString fullPath = dir.absoluteFilePath(team + "_" + safeFileName);

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Could not save CSV file:" << fullPath << "error:" << file.errorString();
        return;
    }

    const qint64 bytesWritten = file.write(fileData.toUtf8());
    file.close();

    if (bytesWritten <= 0)
    {
        qDebug() << "Could not write CSV data to:" << fullPath;
        return;
    }

    // Parse and import the CSV
    QFile importFile(fullPath);
    if (importFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        parseAndImportCsv(team, importFile);
        importFile.close();
    }

    qDebug() << "CSV file saved and imported:" << fullPath;

    // Refresh CSV file list in browser
    sendSavedCsvFiles();
}

/*
 * Helper method to parse CSV file and import players
 */
void web_server::parseAndImportCsv(const QString &team, QFile &file)
{
    if (!m_controller)
        return;

    QByteArray line;
    QList<std::pair<int, QString>> players;

    while ((line = file.readLine()).size() > 0)
    {
        QString lineStr = QString::fromUtf8(line).trimmed();

        if (lineStr.isEmpty())
        {
            continue;
        }

        // Split by comma or semicolon
        QStringList parts = lineStr.split(QRegularExpression("[,;]"));

        if (parts.size() >= 2)
        {
            bool ok = false;
            int number = parts[0].trimmed().toInt(&ok);
            QString name = parts[1].trimmed();

            if (ok && number >= 0 && !name.isEmpty())
            {
                players.append({number, name});
            }
        }
    }

    // Clear existing players and add new ones
    const auto side =
        (team == "Home") ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;

    if (team == "Home")
    {
        // Clear existing home players
        auto existingPlayers = m_controller->getHomePlayers();
        for (const auto &playerPtr : existingPlayers)
        {
            m_controller->removePlayer(match_controller::TeamSide::Home, playerPtr->getNumber());
        }
    }
    else
    {
        // Clear existing away players
        auto existingPlayers = m_controller->getAwayPlayers();
        for (const auto &playerPtr : existingPlayers)
        {
            m_controller->removePlayer(match_controller::TeamSide::Away, playerPtr->getNumber());
        }
    }

    // Add new players
    for (const auto &player : players)
    {
        m_controller->addPlayer(side, player.first, player.second);
    }

    qDebug() << "Parsed and imported" << players.size() << "players for team:" << team;
}
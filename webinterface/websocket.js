/**
 * @file websocket.js
 * @brief WebSocket client for Anzeigetafel - handles all server communication
 *
 * Provides WebSocket connection management, message sending/receiving,
 * and connection state tracking.
 */

class WebSocketClient {
  constructor(serverUrl = "ws://localhost:8080") {
    this.serverUrl = serverUrl;
    this.socket = null;
    this.reconnectAttempts = 0;
    this.maxReconnectAttempts = 5;
    this.reconnectDelay = 3000;

    // Callback handlers
    this.onConnected = null;
    this.onDisconnected = null;
    this.onError = null;
    this.onMessage = null;
  }

  /**
   * Connect to the WebSocket server
   */
  connect() {
    try {
      this.socket = new WebSocket(this.serverUrl);

      this.socket.addEventListener("open", (event) => this.onOpen(event));
      this.socket.addEventListener("message", (event) => this.onMessage(event));
      this.socket.addEventListener("error", (event) => this.onError(event));
      this.socket.addEventListener("close", (event) => this.onClose(event));

      console.log(`[WebSocket] Connecting to ${this.serverUrl}`);
    } catch (error) {
      console.error("[WebSocket] Connection error:", error);
      if (this.onError) this.onError(error);
      this.scheduleReconnect();
    }
  }

  /**
   * Handle WebSocket connection opened
   */
  onOpen(event) {
    console.log("[WebSocket] Connected to server");
    this.reconnectAttempts = 0;
    if (this.onConnected) this.onConnected();
  }

  /**
   * Handle WebSocket errors
   */
  onWsError(event) {
    console.error("[WebSocket] Error:", event);
    if (this.onError) this.onError(event);
  }

  /**
   * Handle WebSocket connection closed
   */
  onClose(event) {
    console.log("[WebSocket] Connection closed", event.code, event.reason);
    if (this.onDisconnected) this.onDisconnected();
    this.scheduleReconnect();
  }

  /**
   * Schedule reconnection attempt
   */
  scheduleReconnect() {
    if (this.reconnectAttempts < this.maxReconnectAttempts) {
      this.reconnectAttempts++;
      const delay = this.reconnectDelay * this.reconnectAttempts;
      console.log(
        `[WebSocket] Reconnecting in ${delay}ms (attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts})`,
      );

      setTimeout(() => {
        this.connect();
      }, delay);
    } else {
      console.error("[WebSocket] Max reconnection attempts reached");
      if (this.onError) this.onError("Max reconnection attempts reached");
    }
  }

  /**
   * Send message to server
   */
  send(message) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      console.error("[WebSocket] Socket not ready");
      return false;
    }

    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent:", message);
      return true;
    } catch (error) {
      console.error("[WebSocket] Failed to send message:", error);
      return false;
    }
  }

  /**
   * Send match state change request to server
   */
  sendMatchStateChange(stateId) {
    return this.send({
      type: "setMatchState",
      state: stateId,
    });
  }

  /**
   * Send start timer request to server
   */
  sendStartTimer() {
    return this.send({ type: "startTimer" });
  }

  /**
   * Send goal request to server
   */
  sendGoal(
    team,
    playerNumber = null,
    playerName = null,
    goalMinute = null,
    isOwnGoal = false,
  ) {
    const message = { type: "goal", team };
    if (playerNumber !== null) {
      message.playerNumber = playerNumber;
      message.playerName = playerName;
    }
    if (goalMinute !== null && goalMinute !== undefined) {
      message.goalMinute = goalMinute;
    }
    if (isOwnGoal) {
      message.isOwnGoal = isOwnGoal;
    }
    return this.send(message);
  }

  /**
   * Send restart timer request to server
   */
  sendRestartTimer() {
    return this.send({ type: "restartTimer" });
  }

  /**
   * Send reset score and timer request to server
   */
  sendResetScoreAndTimer() {
    return this.send({ type: "resetScoreAndTimer" });
  }

  /**
   * Send retake last goal request to server
   */
  sendRetakeLastGoal() {
    return this.send({ type: "retakeLastGoal" });
  }

  /**
   * Send add player request to server
   */
  sendAddPlayer(team, number, name) {
    return this.send({ type: "addPlayer", team, number, name });
  }

  /**
   * Send import players request to server
   */
  sendImportPlayers(team, players) {
    return this.send({ type: "importPlayers", team, players });
  }

  /**
   * Send remove player request to server
   */
  sendRemovePlayer(team, number) {
    return this.send({ type: "removePlayer", team, number });
  }

  /**
   * Request list of saved emblems from server
   */
  requestSavedEmblems() {
    return this.send({ type: "getSavedEmblems" });
  }

  /**
   * Send selected emblem path to server
   */
  sendSelectSavedEmblem(team, filePath) {
    return this.send({
      type: "selectSavedEmblem",
      team: team,
      filePath: filePath,
    });
  }

  /**
   * Send emblem data to server
   */
  sendEmblem(team, fileName, mimeType, dataUrl) {
    return this.send({
      type: "setEmblem",
      team: team,
      fileName: fileName,
      mimeType: mimeType,
      dataUrl: dataUrl,
    });
  }

  /**
   * Request list of saved CSV files from server
   */
  requestSavedCsvFiles() {
    return this.send({ type: "getSavedCsvFiles" });
  }

  /**
   * Send selected CSV file path to server
   */
  sendSelectSavedCsv(team, filePath) {
    return this.send({
      type: "selectSavedCsv",
      team: team,
      filePath: filePath,
    });
  }

  /**
   * Send CSV file data to server
   */
  sendCsvFile(team, fileName, fileData) {
    return this.send({
      type: "setCsvFile",
      team: team,
      fileName: fileName,
      fileData: fileData,
    });
  }

  /**
   * Request list of files/directories for a matchstate slide folder.
   */
  requestMatchStateSlides(stateKey) {
    return this.send({
      type: "getMatchStateSlides",
      stateKey,
    });
  }

  /**
   * Delete a file from a matchstate slide folder.
   */
  deleteMatchStateSlide(stateKey, entryPath) {
    return this.send({
      type: "deleteMatchStateSlide",
      stateKey,
      entryPath,
    });
  }

  /**
   * Upload a new slide to a matchstate slide folder.
   */
  uploadMatchStateSlide(stateKey, fileName, dataUrl) {
    return this.send({
      type: "uploadMatchStateSlide",
      stateKey,
      fileName,
      dataUrl,
    });
  }

  /**
   * Synchronize server-side internal clock using browser epoch milliseconds.
   */
  sendSetClockTime(epochMs, iso = null) {
    return this.send({
      type: "setClockTime",
      epochMs,
      iso,
    });
  }

  /**
   * Request current server-side internal clock snapshot.
   */
  requestClockTime() {
    return this.send({
      type: "getClockTime",
    });
  }

  /**
   * Check if connected to server
   */
  isConnected() {
    return this.socket && this.socket.readyState === WebSocket.OPEN;
  }

  /**
   * Disconnect from server
   */
  disconnect() {
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
  }
}

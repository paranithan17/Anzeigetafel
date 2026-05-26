/*
 * @file application.js
 * @brief Core application bootstrap for Anzeigetafel match state control
 *
 * Handles websocket lifecycle, shared UI utilities, and feature-module setup.
 * Feature-specific behavior lives in the dedicated module files loaded after this one.
 */

class ApplicationClient {
  constructor(serverUrl = "ws://192.168.200.8:8080") {
    this.wsClient = new WebSocketClient(serverUrl);

    this.stateMap = {
      0: "pregame",
      1: "firsthalf",
      2: "halftime",
      3: "secondhalf",
      4: "postgame",
    };

    this.stateNameMap = {
      0: "Pre game",
      1: "First half",
      2: "Half time",
      3: "Second half",
      4: "Post game",
    };

    this.players = { Home: [], Away: [] };

    this.currentEmblemTeam = null;
    this.currentCsvTeam = null;

    this.requestingTeam = null;
    this.currentMatchState = 0;
    this.pendingGoalData = null;
    this.timerRunning = false;

    this.init();
  }

  /**
   * Initialize the client and connect to WebSocket server.
   */
  init() {
    this.wsClient.onConnected = () => this.onWsConnected();
    this.wsClient.onDisconnected = () => this.onWsDisconnected();
    this.wsClient.onError = (error) => this.onWsError(error);
    this.wsClient.onMessage = (event) => this.onWsMessage(event);

    this.wsClient.connect();

    this.setupMatchStateHandlers();
    this.setupGoalHandlers();
    this.setupPlayerManagementHandlers();
    this.setupEmblemManagementHandlers();
    this.setupLogModalHandlers();
    this.setupTimerHandlers();
    this.setupFullscreenButton();

    this.updateConnectionStatus("connecting");
  }

  /**
   * Handle WebSocket connected callback.
   */
  onWsConnected() {
    console.log("[Application] WebSocket connected");
    this.updateConnectionStatus("connected");
    this.showNotification("Connected to server", "success");
  }

  /**
   * Handle WebSocket disconnected callback.
   */
  onWsDisconnected() {
    console.log("[Application] WebSocket disconnected");
    this.updateConnectionStatus("disconnected");
    this.showNotification("Disconnected from server", "warning");
  }

  /**
   * Handle WebSocket error callback.
   */
  onWsError(error) {
    console.error("[Application] WebSocket error:", error);
    this.updateConnectionStatus("error");
    this.showNotification("WebSocket error occurred", "error");
  }

  /**
   * Handle incoming messages from WebSocket server.
   */
  onWsMessage(event) {
    try {
      const data = JSON.parse(event.data);
      console.log("[Application] Received:", data);

      if (data.type === "matchState") {
        this.handleMatchStateUpdate(data.state, data.stateName);
      } else if (data.type === "scoreTime") {
        this.updateScoreTimeDisplay(data.home, data.away, data.time);
      } else if (data.type === "playersList") {
        this.handlePlayersListUpdate(data.team, data.players);
      } else if (data.type === "savedEmblemsList") {
        this.displaySavedEmblems(data.emblems);
      } else if (data.type === "savedCsvFilesList") {
        this.displaySavedCsvFiles(data.files);
      }
    } catch (error) {
      console.error("[Application] Failed to parse message:", error, event.data);
    }
  }

  /**
   * Update connection status indicator.
   */
  updateConnectionStatus(status) {
    const statusElement = document.getElementById("connectionStatus");
    if (statusElement) {
      statusElement.className = `connection-status status-${status}`;

      const statusText = {
        connecting: "Connecting...",
        connected: "Connected",
        disconnected: "Disconnected",
        error: "Error",
        offline: "Offline",
      };

      statusElement.textContent = statusText[status] || status;
    }
  }

  /**
   * Show notification message.
   */
  showNotification(message, type = "info") {
    console.log(`[Notification] ${type.toUpperCase()}: ${message}`);

    const notification = document.createElement("div");
    notification.className = `notification notification-${type}`;
    notification.textContent = message;

    const container = document.body;
    container.appendChild(notification);

    setTimeout(() => {
      notification.classList.add("fade-out");
      setTimeout(() => {
        if (notification.parentNode === container) {
          container.removeChild(notification);
        }
      }, 500);
    }, 5000);
  }

  /**
   * Show modal dialog.
   */
  showModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
      modal.style.display = "flex";
      modal.classList.add("show");
    }
  }

  /**
   * Hide modal dialog.
   */
  hideModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
      modal.style.display = "none";
      modal.classList.remove("show");
    }
  }

  /**
   * Setup fullscreen button.
   */
  setupFullscreenButton() {
    const fullscreenBtn = document.getElementById("fullscreenBtn");

    if (!fullscreenBtn) {
      return;
    }

    fullscreenBtn.addEventListener("click", async () => {
      try {
        if (!document.fullscreenElement) {
          await document.documentElement.requestFullscreen();
          fullscreenBtn.textContent = "⛶";
          fullscreenBtn.title = "Fullscreen verlassen";
        } else {
          await document.exitFullscreen();
          fullscreenBtn.textContent = "⛶";
          fullscreenBtn.title = "Fullscreen öffnen";
        }
      } catch (error) {
        console.error("[Fullscreen] Failed:", error);
        this.showNotification(
          "Fullscreen konnte nicht gestartet werden",
          "error",
        );
      }
    });

    document.addEventListener("fullscreenchange", () => {
      if (document.fullscreenElement) {
        fullscreenBtn.title = "Fullscreen verlassen";
      } else {
        fullscreenBtn.title = "Fullscreen öffnen";
      }
    });
  }

  /**
   * Get current connection state.
   */
  isConnected() {
    return this.wsClient.isConnected();
  }

  /**
   * Disconnect and cleanup.
   */
  disconnect() {
    this.wsClient.disconnect();
  }
}

// Initialize client when DOM is ready.
document.addEventListener("DOMContentLoaded", () => {
  console.log("[Application] Initializing Anzeigetafel Application");

  window.anzeigetafelClient = new ApplicationClient("ws://192.168.200.8:8080");

  console.log("[Application] Client initialized");
});

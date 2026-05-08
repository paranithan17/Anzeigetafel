/**
 * @file app.js
 * @brief WebSocket client for Anzeigetafel match state control
 *
 * Communicates with the C++ WebSocket server to:
 * - Send match state change requests
 * - Receive and display match state updates
 * - Handle connection events
 */

class AnzeigetafelClient {
  constructor(serverUrl = "ws://localhost:8080") {
    this.serverUrl = serverUrl;
    this.socket = null;
    this.reconnectAttempts = 0;
    this.maxReconnectAttempts = 5;
    this.reconnectDelay = 3000;

    // State mapping
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

    this.init();
  }

  /**
   * Initialize the client and connect to WebSocket server
   */
  init() {
    this.connect();
    this.setupEventListeners();
    this.updateConnectionStatus("connecting");
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
      this.updateConnectionStatus("error");
      this.scheduleReconnect();
    }
  }

  /**
   * Handle WebSocket connection opened
   */
  onOpen(event) {
    console.log("[WebSocket] Connected to server");
    this.reconnectAttempts = 0;
    this.updateConnectionStatus("connected");
    this.showNotification("Connected to server", "success");
  }

  /**
   * Handle incoming messages from server
   */
  onMessage(event) {
    try {
      const data = JSON.parse(event.data);
      console.log("[WebSocket] Received:", data);

      if (data.type === "matchState") {
        this.handleMatchStateUpdate(data.state, data.stateName);
      }
    } catch (error) {
      console.error("[WebSocket] Failed to parse message:", error, event.data);
    }
  }

  /**
   * Handle WebSocket errors
   */
  onError(event) {
    console.error("[WebSocket] Error:", event);
    this.updateConnectionStatus("error");
    this.showNotification("WebSocket error occurred", "error");
  }

  /**
   * Handle WebSocket connection closed
   */
  onClose(event) {
    console.log("[WebSocket] Connection closed", event.code, event.reason);
    this.updateConnectionStatus("disconnected");
    this.showNotification("Disconnected from server", "warning");
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
      this.updateConnectionStatus("offline");
    }
  }

  /**
   * Send match state change request to server
   */
  sendMatchStateChange(stateId) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      console.error("[WebSocket] Socket not ready");
      this.showNotification("Not connected to server", "error");
      return false;
    }

    const message = {
      type: "setMatchState",
      state: stateId,
    };

    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent:", message);
      return true;
    } catch (error) {
      console.error("[WebSocket] Failed to send message:", error);
      this.showNotification("Failed to send state change", "error");
      return false;
    }
  }

  /**
   * Handle match state update from server
   */
  handleMatchStateUpdate(stateId, stateName) {
    console.log(`[State Update] State changed to ${stateName} (${stateId})`);

    // Update radio button selection
    const radioId = this.stateMap[stateId];
    if (radioId) {
      const radioElement = document.getElementById(radioId);
      if (radioElement) {
        radioElement.checked = true;
        console.log(`[UI] Updated radio button to: ${radioId}`);
      }
    }

    // Update UI display
    this.updateStateDisplay(stateId, stateName);

    // Enable Start Timer only in FirstHalf (1) or SecondHalf (3)
    const startBtn = document.getElementById("startTimerBtn");
    if (startBtn) {
      const enabled = stateId === 1 || stateId === 3;
      startBtn.disabled = !enabled;
    }
  }

  /**
   * Update state display on UI
   */
  updateStateDisplay(stateId, stateName) {
    const stateDisplay = document.getElementById("currentStateDisplay");
    if (stateDisplay) {
      stateDisplay.textContent =
        stateName || this.stateNameMap[stateId] || "Unknown";
      stateDisplay.className = `state-display state-${stateId}`;
    }
  }

  /**
   * Update connection status indicator
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
   * Show notification message
   */
  showNotification(message, type = "info") {
    console.log(`[Notification] ${type.toUpperCase()}: ${message}`);

    const notification = document.createElement("div");
    notification.className = `notification notification-${type}`;
    notification.textContent = message;

    const container = document.body;
    container.appendChild(notification);

    // Auto-remove after 5 seconds
    setTimeout(() => {
      notification.classList.add("fade-out");
      setTimeout(() => {
        container.removeChild(notification);
      }, 500);
    }, 5000);
  }

  /**
   * Setup event listeners for form and controls
   */
  setupEventListeners() {
    // Form submission
    const form = document.getElementById("matchStateForm");
    if (form) {
      form.addEventListener("submit", (e) => {
        e.preventDefault();
        this.handleFormSubmit();
      });
    }

    // Individual radio button changes (optional: update on click)
    const radioButtons = document.querySelectorAll('input[name="matchState"]');
    radioButtons.forEach((radio) => {
      radio.addEventListener("change", (e) => {
        console.log(`[UI] Radio selected: ${e.target.value}`);
      });
    });

    // Goal buttons
    const homeGoal = document.getElementById("homeGoalBtn");
    if (homeGoal) {
      homeGoal.addEventListener("click", () => {
        console.log("[UI] Home Goal button clicked");
        this.sendGoal("Home");
      });
    }

    const awayGoal = document.getElementById("awayGoalBtn");
    if (awayGoal) {
      awayGoal.addEventListener("click", () => {
        console.log("[UI] Away Goal button clicked");
        this.sendGoal("Away");
      });
    }

    // Start timer button
    const startBtn = document.getElementById("startTimerBtn");
    if (startBtn) {
      startBtn.addEventListener("click", () => {
        console.log("[UI] Start Timer clicked");
        this.sendStartTimer();
      });
    }
  }

  /**
   * Send start timer request to server
   */
  sendStartTimer() {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) return false;

    const message = { type: "startTimer" };
    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent startTimer");
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send startTimer", err);
      return false;
    }
  }

  /**
   * Send minimal goal request to server
   */
  sendGoal(team) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) return false;

    const message = { type: "goal", team };
    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent goal for", team);
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send goal", err);
      return false;
    }
  }

  /**
   * Handle form submission
   */
  handleFormSubmit() {
    const selectedRadio = document.querySelector(
      'input[name="matchState"]:checked',
    );

    if (!selectedRadio) {
      this.showNotification("Please select a match state", "warning");
      return;
    }

    const value = selectedRadio.value;

    // Find state ID for the value
    let stateId = null;
    for (const [id, name] of Object.entries(this.stateMap)) {
      if (name === value) {
        stateId = parseInt(id);
        break;
      }
    }

    if (stateId === null) {
      this.showNotification("Invalid state selected", "error");
      return;
    }

    console.log(`[Form] Submitting state change: ${value} (${stateId})`);
    this.sendMatchStateChange(stateId);
  }

  /**
   * Get current connection state
   */
  isConnected() {
    return this.socket && this.socket.readyState === WebSocket.OPEN;
  }

  /**
   * Disconnect and cleanup
   */
  disconnect() {
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
  }
}

// Initialize client when DOM is ready
document.addEventListener("DOMContentLoaded", () => {
  console.log("[App] Initializing Anzeigetafel client");

  // Create global client instance
  window.anzeigetafelClient = new AnzeigetafelClient("ws://192.168.1.36:8080");

  console.log("[App] Client initialized");
});

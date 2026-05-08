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

    // Keep the latest players lists in memory for quick access
    this.players = { Home: [], Away: [] };

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
      } else if (data.type === "playersList") {
        this.handlePlayersListUpdate(data.team, data.players);
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
        this.showGoalSelector("Home");
      });
    }

    const awayGoal = document.getElementById("awayGoalBtn");
    if (awayGoal) {
      awayGoal.addEventListener("click", () => {
        console.log("[UI] Away Goal button clicked");
        this.showGoalSelector("Away");
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

    // Home team player handlers
    const homeImportFile = document.getElementById("homeImportFile");
    if (homeImportFile) {
      homeImportFile.addEventListener("change", (e) => {
        this.handlePlayerImportFile(e.target.files[0], "Home");
        homeImportFile.value = "";
      });
    }

    const homeImportBtn = document.getElementById("homeImportBtn");
    if (homeImportBtn) {
      homeImportBtn.addEventListener("click", () => {
        document.getElementById("homeImportFile").click();
      });
    }

    const homeAddPlayerShowBtn = document.getElementById(
      "homeAddPlayerShowBtn",
    );
    if (homeAddPlayerShowBtn) {
      homeAddPlayerShowBtn.addEventListener("click", () => {
        this.showModal("homeAddPlayerModal");
      });
    }

    const homeModalClose = document.getElementById("homeModalClose");
    if (homeModalClose) {
      homeModalClose.addEventListener("click", () => {
        this.hideModal("homeAddPlayerModal");
      });
    }

    const homeAddPlayerCancelBtn = document.getElementById(
      "homeAddPlayerCancelBtn",
    );
    if (homeAddPlayerCancelBtn) {
      homeAddPlayerCancelBtn.addEventListener("click", () => {
        this.hideModal("homeAddPlayerModal");
      });
    }

    const homeAddPlayerConfirmBtn = document.getElementById(
      "homeAddPlayerConfirmBtn",
    );
    if (homeAddPlayerConfirmBtn) {
      homeAddPlayerConfirmBtn.addEventListener("click", () => {
        const num = document.getElementById("homePlayerNum").value;
        const name = document.getElementById("homePlayerName").value;
        if (num && name) {
          this.sendAddPlayer("Home", parseInt(num), name);
          document.getElementById("homePlayerNum").value = "";
          document.getElementById("homePlayerName").value = "";
          this.hideModal("homeAddPlayerModal");
        } else {
          this.showNotification("Please enter number and name", "warning");
        }
      });
    }

    // Away team player handlers
    const awayImportFile = document.getElementById("awayImportFile");
    if (awayImportFile) {
      awayImportFile.addEventListener("change", (e) => {
        this.handlePlayerImportFile(e.target.files[0], "Away");
        awayImportFile.value = "";
      });
    }

    const awayImportBtn = document.getElementById("awayImportBtn");
    if (awayImportBtn) {
      awayImportBtn.addEventListener("click", () => {
        document.getElementById("awayImportFile").click();
      });
    }

    const awayAddPlayerShowBtn = document.getElementById(
      "awayAddPlayerShowBtn",
    );
    if (awayAddPlayerShowBtn) {
      awayAddPlayerShowBtn.addEventListener("click", () => {
        this.showModal("awayAddPlayerModal");
      });
    }

    const awayModalClose = document.getElementById("awayModalClose");
    if (awayModalClose) {
      awayModalClose.addEventListener("click", () => {
        this.hideModal("awayAddPlayerModal");
      });
    }

    const awayAddPlayerCancelBtn = document.getElementById(
      "awayAddPlayerCancelBtn",
    );
    if (awayAddPlayerCancelBtn) {
      awayAddPlayerCancelBtn.addEventListener("click", () => {
        this.hideModal("awayAddPlayerModal");
      });
    }

    const awayAddPlayerConfirmBtn = document.getElementById(
      "awayAddPlayerConfirmBtn",
    );
    if (awayAddPlayerConfirmBtn) {
      awayAddPlayerConfirmBtn.addEventListener("click", () => {
        const num = document.getElementById("awayPlayerNum").value;
        const name = document.getElementById("awayPlayerName").value;
        if (num && name) {
          this.sendAddPlayer("Away", parseInt(num), name);
          document.getElementById("awayPlayerNum").value = "";
          document.getElementById("awayPlayerName").value = "";
          this.hideModal("awayAddPlayerModal");
        } else {
          this.showNotification("Please enter number and name", "warning");
        }
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
   * Handle CSV file import (read and send to server)
   */
  handlePlayerImportFile(file, team) {
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      try {
        const csv = e.target.result;
        const lines = csv.split("\n").filter((line) => line.trim());
        const players = [];

        for (const line of lines) {
          const [numStr, name] = line.split(/[,;]/).map((s) => s.trim());
          const num = parseInt(numStr);
          if (!isNaN(num) && name) {
            players.push({ number: num, name });
          }
        }

        if (players.length > 0) {
          this.sendImportPlayers(team, players);
          console.log(
            `[Players] Imported ${players.length} players for ${team} team`,
          );
        }
      } catch (err) {
        console.error("[Players] Failed to parse CSV:", err);
        this.showNotification(`Failed to parse CSV for ${team}`, "error");
      }
    };
    reader.readAsText(file);
  }

  /**
   * Send add player request to server
   */
  sendAddPlayer(team, number, name) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) return false;

    const message = { type: "addPlayer", team, number, name };
    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent addPlayer:", team, number, name);
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send addPlayer", err);
      return false;
    }
  }

  /**
   * Send import players request to server
   */
  sendImportPlayers(team, players) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) return false;

    const message = { type: "importPlayers", team, players };
    try {
      this.socket.send(JSON.stringify(message));
      console.log(
        "[WebSocket] Sent importPlayers:",
        team,
        players.length,
        "players",
      );
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send importPlayers", err);
      return false;
    }
  }

  /**
   * Handle players list update from server
   */
  handlePlayersListUpdate(team, players) {
    console.log(
      `[Players] Received ${players.length} players for ${team} team`,
    );
    // store latest players in memory
    this.players[team] = Array.isArray(players) ? players : [];
    this.displayPlayersList(team, this.players[team]);
  }

  /**
   * Display player list in UI
   */
  displayPlayersList(team, players) {
    const listId = team === "Home" ? "homePlayersList" : "awayPlayersList";
    const listElement = document.getElementById(listId);
    if (!listElement) return;

    if (!players || players.length === 0) {
      listElement.innerHTML =
        '<p style="color:#666; font-size:12px;">No players</p>';
      return;
    }

    let html = "";
    for (const player of players) {
      html += `<div style="padding:6px; border-bottom:1px solid #333; display:flex; justify-content:space-between; align-items:center; color:#ddd; font-size:12px;">
        <span><strong>#${player.number}</strong> ${player.name}</span>
        <button class="player-remove-btn" data-team="${team}" data-number="${player.number}" style="padding:2px 8px; background:#555; border:none; color:#fff; cursor:pointer; border-radius:3px; font-size:10px;">Remove</button>
      </div>`;
    }
    listElement.innerHTML = html;

    // Attach remove handlers
    const removeBtns = listElement.querySelectorAll(".player-remove-btn");
    removeBtns.forEach((btn) => {
      btn.addEventListener("click", (e) => {
        const t = e.target.dataset.team;
        const n = parseInt(e.target.dataset.number);
        this.sendRemovePlayer(t, n);
      });
    });
  }

  /**
   * Populate the goal selection modal lists with current players
   */
  populateGoalSelectionModal() {
    const homeList = document.getElementById("goalHomeList");
    const awayList = document.getElementById("goalAwayList");
    if (!homeList || !awayList) return;

    const renderList = (element, team) => {
      const players = this.players[team] || [];
      if (!players || players.length === 0) {
        element.innerHTML =
          '<p style="color:#666; font-size:12px;">No players</p>';
        return;
      }

      let html = "";
      for (const player of players) {
        html += `<div style="padding:6px; border-bottom:1px solid #333; display:flex; justify-content:space-between; align-items:center;">
            <span style="color:#ddd;"><strong>#${player.number}</strong> ${player.name}</span>
            <button class="scorer-btn" data-team="${team}" data-number="${player.number}" data-name="${player.name}" style="padding:6px 10px; background:#007acc; border:none; color:#fff; cursor:pointer; border-radius:3px;">Select</button>
          </div>`;
      }
      element.innerHTML = html;

      // attach handlers
      const btns = element.querySelectorAll(".scorer-btn");
      btns.forEach((btn) => {
        btn.addEventListener("click", (e) => {
          const t = e.currentTarget.dataset.team;
          const n = parseInt(e.currentTarget.dataset.number);
          const name = e.currentTarget.dataset.name;
          this.sendGoalWithPlayer(t, { number: n, name });
          this.hideModal("goalSelectModal");
        });
      });
    };

    renderList(homeList, "Home");
    renderList(awayList, "Away");
  }

  /**
   * Show the goal selector modal and populate with players
   */
  showGoalSelector(requestingTeam) {
    // ensure lists are up to date
    this.populateGoalSelectionModal();
    // show modal
    this.showModal("goalSelectModal");

    // close and cancel handlers
    const closeBtn = document.getElementById("goalModalClose");
    if (closeBtn) {
      closeBtn.onclick = () => this.hideModal("goalSelectModal");
    }
    const cancelBtn = document.getElementById("goalCancelBtn");
    if (cancelBtn) {
      cancelBtn.onclick = () => this.hideModal("goalSelectModal");
    }
  }

  /**
   * Send goal event including player info
   */
  sendGoalWithPlayer(team, player) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      this.showNotification("Not connected to server", "error");
      return false;
    }

    const message = {
      type: "goal",
      team,
      playerNumber: player.number,
      playerName: player.name,
    };
    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent goal for", team, player);
      this.showNotification(
        `${player.name} (#${player.number}) recorded for ${team}`,
        "success",
      );
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send goal", err);
      this.showNotification("Failed to send goal", "error");
      return false;
    }
  }

  /**
   * Send remove player request to server
   */
  sendRemovePlayer(team, number) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) return false;

    const message = { type: "removePlayer", team, number };
    try {
      this.socket.send(JSON.stringify(message));
      console.log("[WebSocket] Sent removePlayer:", team, number);
      return true;
    } catch (err) {
      console.error("[WebSocket] Failed to send removePlayer", err);
      return false;
    }
  }

  /**
   * Show modal dialog
   */
  showModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
      modal.style.display = "flex";
      modal.classList.add("show");
    }
  }

  /**
   * Hide modal dialog
   */
  hideModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
      modal.style.display = "none";
      modal.classList.remove("show");
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

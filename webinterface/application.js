/**
 * @file application.js
 * @brief Application logic for Anzeigetafel match state control
 *
 * Handles UI interactions, form submissions, and application state management.
 * Uses WebSocketClient for server communication.
 */

class ApplicationClient {
  constructor(serverUrl = "ws://localhost:8080") {
    this.wsClient = new WebSocketClient(serverUrl);

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

    this.currentEmblemTeam = null;

    this.init();
  }

  /**
   * Initialize the client and connect to WebSocket server
   */
  init() {
    // Setup WebSocket callbacks
    this.wsClient.onConnected = () => this.onWsConnected();
    this.wsClient.onDisconnected = () => this.onWsDisconnected();
    this.wsClient.onError = (error) => this.onWsError(error);
    this.wsClient.onMessage = (event) => this.onWsMessage(event);

    this.wsClient.connect();
    this.setupEventListeners();
    this.setupFullscreenButton();
    this.updateConnectionStatus("connecting");
  }

  /**
   * Handle WebSocket connected callback
   */
  onWsConnected() {
    console.log("[Application] WebSocket connected");
    this.updateConnectionStatus("connected");
    this.showNotification("Connected to server", "success");
  }

  /**
   * Handle WebSocket disconnected callback
   */
  onWsDisconnected() {
    console.log("[Application] WebSocket disconnected");
    this.updateConnectionStatus("disconnected");
    this.showNotification("Disconnected from server", "warning");
  }

  /**
   * Handle WebSocket error callback
   */
  onWsError(error) {
    console.error("[Application] WebSocket error:", error);
    this.updateConnectionStatus("error");
    this.showNotification("WebSocket error occurred", "error");
  }

  /**
   * Handle incoming messages from WebSocket server
   */
  onWsMessage(event) {
    try {
      const data = JSON.parse(event.data);
      console.log("[Application] Received:", data);

      if (data.type === "matchState") {
        this.handleMatchStateUpdate(data.state, data.stateName);
      } else if (data.type === "playersList") {
        this.handlePlayersListUpdate(data.team, data.players);
      } else if (data.type === "savedEmblemsList") {
        this.displaySavedEmblems(data.emblems);
      }
    } catch (error) {
      console.error(
        "[Application] Failed to parse message:",
        error,
        event.data,
      );
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
        if (this.wsClient.isConnected()) {
          this.wsClient.sendStartTimer();
        } else {
          this.showNotification("Not connected to server", "error");
        }
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

    const homeEmblemBtn = document.getElementById("homeEmblemBtn");
    const homeEmblemFile = document.getElementById("homeEmblemFile");

    if (homeEmblemBtn && homeEmblemFile) {
      homeEmblemBtn.addEventListener("click", () => {
        this.openEmblemModal("Home");
      });

      homeEmblemFile.addEventListener("change", (e) => {
        this.handleEmblemUpload(e.target.files[0], "Home");
        homeEmblemFile.value = "";
      });
    }

    const awayEmblemBtn = document.getElementById("awayEmblemBtn");
    const awayEmblemFile = document.getElementById("awayEmblemFile");

    if (awayEmblemBtn && awayEmblemFile) {
      awayEmblemBtn.addEventListener("click", () => {
        this.openEmblemModal("Away");
      });

      awayEmblemFile.addEventListener("change", (e) => {
        this.handleEmblemUpload(e.target.files[0], "Away");
        awayEmblemFile.value = "";
      });
    }

    const emblemModalClose = document.getElementById("emblemModalClose");
    if (emblemModalClose) {
      emblemModalClose.addEventListener("click", () => {
        this.hideModal("emblemSelectModal");
      });
    }

    const emblemCancelBtn = document.getElementById("emblemCancelBtn");
    if (emblemCancelBtn) {
      emblemCancelBtn.addEventListener("click", () => {
        this.hideModal("emblemSelectModal");
      });
    }

    const uploadNewEmblemBtn = document.getElementById("uploadNewEmblemBtn");
    if (uploadNewEmblemBtn) {
      uploadNewEmblemBtn.addEventListener("click", () => {
        if (this.currentEmblemTeam === "Home") {
          homeEmblemFile.click();
        } else if (this.currentEmblemTeam === "Away") {
          awayEmblemFile.click();
        }
      });
    }
  }
  /**
   * Setup fullscreen button
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
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }
    return this.wsClient.sendAddPlayer(team, number, name);
  }

  /**
   * Send import players request to server
   */
  sendImportPlayers(team, players) {
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }
    return this.wsClient.sendImportPlayers(team, players);
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
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }

    const success = this.wsClient.sendGoal(team, player.number, player.name);
    if (success) {
      this.showNotification(
        `${player.name} (#${player.number}) recorded for ${team}`,
        "success",
      );
    } else {
      this.showNotification("Failed to send goal", "error");
    }
    return success;
  }

  /**
   * Send remove player request to server
   */
  sendRemovePlayer(team, number) {
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }
    return this.wsClient.sendRemovePlayer(team, number);
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

    console.log(`[Application] Submitting state change: ${value} (${stateId})`);
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return;
    }
    this.wsClient.sendMatchStateChange(stateId);
  }

  /**
   * Get current connection state
   */
  isConnected() {
    return this.wsClient.isConnected();
  }

  /**
   * Disconnect and cleanup
   */
  disconnect() {
    this.wsClient.disconnect();
  }
  /**
   * Open emblem selection modal for selected team.
   */
  openEmblemModal(team) {
    this.currentEmblemTeam = team;

    this.showModal("emblemSelectModal");
    this.requestSavedEmblems();
  }

  /**
   * Request list of saved emblems from C++ server.
   */
  requestSavedEmblems() {
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }

    return this.wsClient.requestSavedEmblems();
  }

  /**
   * Display saved emblems in modal.
   */
  displaySavedEmblems(emblems) {
    const listElement = document.getElementById("savedEmblemsList");

    if (!listElement) {
      return;
    }

    if (!emblems || emblems.length === 0) {
      listElement.innerHTML =
        '<p style="color:#666; font-size:12px;">No saved emblems</p>';
      return;
    }

    let html = "";

    for (const emblem of emblems) {
      html += `
      <div class="emblem-choice" data-filepath="${emblem.filePath}">
        <img src="${emblem.data}" alt="${emblem.fileName}">
        <span>${emblem.fileName}</span>
      </div>
    `;
    }

    listElement.innerHTML = html;

    const choices = listElement.querySelectorAll(".emblem-choice");

    choices.forEach((choice) => {
      choice.addEventListener("click", () => {
        const filePath = choice.dataset.filepath;
        this.sendSelectSavedEmblem(this.currentEmblemTeam, filePath);
        this.hideModal("emblemSelectModal");
      });
    });
  }

  /**
   * Send selected saved emblem path to C++ server.
   */
  sendSelectSavedEmblem(team, filePath) {
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }

    const success = this.wsClient.sendSelectSavedEmblem(team, filePath);
    if (success) {
      this.showNotification(`${team} emblem selected`, "success");
    }
    return success;
  }

  /**
   * Read selected emblem file and sent it to server.
   */
  handleEmblemUpload(file, team) {
    if (!file) {
      return;
    }

    if (!file.type.startsWith("image/")) {
      this.showNotification("Please select a valid image file", "warning");
      return;
    }

    const reader = new FileReader();
    reader.onload = (e) => {
      const dataUrl = e.target.result;
      this.sendEmblem(team, file.name, file.type, dataUrl);
    };
    reader.onerror = () => {
      this.showNotification("Failed to read emblem file", "error");
    };
    reader.readAsDataURL(file);
  }

  /**
   * Send emblem data to server
   */
  sendEmblem(team, fileName, mimeType, dataUrl) {
    if (!this.wsClient.isConnected()) {
      this.showNotification("Not connected to server", "error");
      return false;
    }

    const success = this.wsClient.sendEmblem(team, fileName, mimeType, dataUrl);
    if (success) {
      this.showNotification(
        `Emblem "${fileName}" uploaded for ${team}`,
        "success",
      );
    } else {
      this.showNotification("Failed to upload emblem", "error");
    }
    return success;
  }
}

// Initialize client when DOM is ready
document.addEventListener("DOMContentLoaded", () => {
  console.log("[Application] Initializing Anzeigetafel Application");

  // Create global client instance
  window.anzeigetafelClient = new ApplicationClient("ws://192.168.1.36:8080");

  console.log("[Application] Client initialized");
});

/* global ApplicationClient */

ApplicationClient.prototype.setupPlayerManagementHandlers = function () {
  const homeImportFile = document.getElementById("homeImportFile");
  if (homeImportFile) {
    homeImportFile.addEventListener("change", (e) => {
      this.handleCsvUpload(e.target.files[0], "Home");
      homeImportFile.value = "";
    });
  }

  const homeImportBtn = document.getElementById("homeImportBtn");
  if (homeImportBtn) {
    homeImportBtn.addEventListener("click", () => {
      this.openCsvModal("Home");
    });
  }

  const homeAddPlayerShowBtn = document.getElementById("homeAddPlayerShowBtn");
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
        this.sendAddPlayer("Home", parseInt(num, 10), name);
        document.getElementById("homePlayerNum").value = "";
        document.getElementById("homePlayerName").value = "";
        this.hideModal("homeAddPlayerModal");
      } else {
        this.showNotification("Please enter number and name", "warning");
      }
    });
  }

  const awayImportFile = document.getElementById("awayImportFile");
  if (awayImportFile) {
    awayImportFile.addEventListener("change", (e) => {
      this.handleCsvUpload(e.target.files[0], "Away");
      awayImportFile.value = "";
    });
  }

  const awayImportBtn = document.getElementById("awayImportBtn");
  if (awayImportBtn) {
    awayImportBtn.addEventListener("click", () => {
      this.openCsvModal("Away");
    });
  }

  const awayAddPlayerShowBtn = document.getElementById("awayAddPlayerShowBtn");
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
        this.sendAddPlayer("Away", parseInt(num, 10), name);
        document.getElementById("awayPlayerNum").value = "";
        document.getElementById("awayPlayerName").value = "";
        this.hideModal("awayAddPlayerModal");
      } else {
        this.showNotification("Please enter number and name", "warning");
      }
    });
  }

  const csvModalClose = document.getElementById("csvModalClose");
  if (csvModalClose) {
    csvModalClose.addEventListener("click", () => {
      this.hideModal("csvSelectModal");
    });
  }

  const csvCancelBtn = document.getElementById("csvCancelBtn");
  if (csvCancelBtn) {
    csvCancelBtn.addEventListener("click", () => {
      this.hideModal("csvSelectModal");
    });
  }

  const uploadNewCsvBtn = document.getElementById("uploadNewCsvBtn");
  if (uploadNewCsvBtn) {
    uploadNewCsvBtn.addEventListener("click", () => {
      if (this.currentCsvTeam === "Home" && homeImportFile) {
        homeImportFile.click();
      } else if (this.currentCsvTeam === "Away" && awayImportFile) {
        awayImportFile.click();
      }
    });
  }
};

ApplicationClient.prototype.handlePlayerImportFile = function (file, team) {
  return this.handleCsvUpload(file, team);
};

ApplicationClient.prototype.handlePlayersListUpdate = function (team, players) {
  const playerCount = Array.isArray(players) ? players.length : 0;
  console.log(`[Players] Received ${playerCount} players for ${team} team`);
  this.players[team] = Array.isArray(players) ? players : [];
  this.displayPlayersList(team, this.players[team]);
};

ApplicationClient.prototype.displayPlayersList = function (team, players) {
  const listId = team === "Home" ? "homePlayersList" : "awayPlayersList";
  const listElement = document.getElementById(listId);
  if (!listElement) {
    return;
  }

  if (!players || players.length === 0) {
    listElement.innerHTML = '<p class="empty-list-message">No players</p>';
    return;
  }

  let html = "";
  for (const player of players) {
    html += `<div class="player-list-item">
      <span><strong>#${player.number}</strong> ${player.name}</span>
      <button class="player-remove-btn player-remove-button" data-team="${team}" data-number="${player.number}">Remove</button>
    </div>`;
  }
  listElement.innerHTML = html;

  const removeBtns = listElement.querySelectorAll(".player-remove-btn");
  removeBtns.forEach((btn) => {
    btn.addEventListener("click", (e) => {
      const selectedTeam = e.target.dataset.team;
      const number = parseInt(e.target.dataset.number, 10);
      this.sendRemovePlayer(selectedTeam, number);
    });
  });
};

ApplicationClient.prototype.sendAddPlayer = function (team, number, name) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.sendAddPlayer(team, number, name);
};

ApplicationClient.prototype.sendImportPlayers = function (team, players) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.sendImportPlayers(team, players);
};

ApplicationClient.prototype.sendRemovePlayer = function (team, number) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.sendRemovePlayer(team, number);
};

ApplicationClient.prototype.openCsvModal = function (team) {
  this.currentCsvTeam = team;
  this.showModal("csvSelectModal");
  this.requestSavedCsvFiles();
};

ApplicationClient.prototype.requestSavedCsvFiles = function () {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.requestSavedCsvFiles();
};

ApplicationClient.prototype.displaySavedCsvFiles = function (files) {
  const listElement = document.getElementById("savedCsvList");
  if (!listElement) {
    return;
  }

  if (!files || files.length === 0) {
    listElement.innerHTML = '<p class="empty-list-message">No saved lists</p>';
    return;
  }

  let html = "";
  for (const file of files) {
    html += `
      <div class="csv-choice" data-filepath="${file.filePath}">
        <span>${file.fileName}</span>
      </div>
    `;
  }

  listElement.innerHTML = html;

  const choices = listElement.querySelectorAll(".csv-choice");
  choices.forEach((choice) => {
    choice.addEventListener("click", () => {
      const filePath = choice.dataset.filepath;
      this.sendSelectSavedCsv(this.currentCsvTeam, filePath);
      this.hideModal("csvSelectModal");
    });
  });
};

ApplicationClient.prototype.sendSelectSavedCsv = function (team, filePath) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  const success = this.wsClient.sendSelectSavedCsv(team, filePath);
  if (success) {
    this.showNotification(`${team} player list selected`, "success");
  }
  return success;
};

ApplicationClient.prototype.handleCsvUpload = function (file, team) {
  if (!file) {
    return;
  }

  const reader = new FileReader();
  reader.onload = (e) => {
    const fileData = e.target.result;
    this.sendCsvFile(team, file.name, fileData);
  };
  reader.onerror = () => {
    this.showNotification("Failed to read CSV file", "error");
  };
  reader.readAsText(file);
};

ApplicationClient.prototype.sendCsvFile = function (team, fileName, fileData) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  const success = this.wsClient.sendCsvFile(team, fileName, fileData);
  if (success) {
    this.showNotification(
      `Player list "${fileName}" uploaded for ${team}`,
      "success",
    );
  } else {
    this.showNotification("Failed to upload player list", "error");
  }

  return success;
};

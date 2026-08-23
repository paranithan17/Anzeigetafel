/* global ApplicationClient */

ApplicationClient.prototype.setupLogModalHandlers = function () {
  const logBtn = document.getElementById("logBtn");
  if (logBtn) {
    logBtn.addEventListener("click", () => {
      console.log("[UI] Log button clicked");
      this.showLogMenu();
    });
  }
};

ApplicationClient.prototype.showLogMenu = function () {
  this.showModal("logMenuModal");

  const restartTimerBtn = document.getElementById("logRestartTimerBtn");
  if (restartTimerBtn) {
    restartTimerBtn.onclick = () => {
      this.hideModal("logMenuModal");
      this.requestRestartTimer();
    };
  }

  const resetScoreTimerBtn = document.getElementById("logResetScoreTimerBtn");
  if (resetScoreTimerBtn) {
    resetScoreTimerBtn.onclick = () => {
      this.hideModal("logMenuModal");
      this.requestResetScoreAndTimer();
    };
  }

  const retakeGoalBtn = document.getElementById("logRetakeGoalBtn");
  if (retakeGoalBtn) {
    retakeGoalBtn.onclick = () => {
      this.hideModal("logMenuModal");
      this.requestRetakeLastGoal();
    };
  }

  const syncClockBtn = document.getElementById("syncClockBtn");
  if (syncClockBtn) {
    syncClockBtn.onclick = () => {
      this.syncClockWithServer(true);
    };
  }

  const cancelBtn = document.getElementById("logMenuCancelBtn");
  if (cancelBtn) {
    cancelBtn.onclick = () => {
      this.hideModal("logMenuModal");
    };
  }

  const closeBtn = document.getElementById("logMenuModalClose");
  if (closeBtn) {
    closeBtn.onclick = () => {
      this.hideModal("logMenuModal");
    };
  }
};

ApplicationClient.prototype.requestRestartTimer = function () {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return;
  }

  this.wsClient.sendRestartTimer();
  this.showNotification("Timer restarted", "success");
  this.timerRunning = true;
  this.updateStartTimerButtonState();
};

ApplicationClient.prototype.requestResetScoreAndTimer = function () {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return;
  }

  const sent = this.wsClient.sendResetScoreAndTimer();
  if (sent) {
    this.timerRunning = false;
    this.updateStartTimerButtonState();
    this.showNotification("Score and timer reset", "success");
  } else {
    this.showNotification("Failed to reset score and timer", "error");
  }
};

ApplicationClient.prototype.requestRetakeLastGoal = function () {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return;
  }

  this.wsClient.sendRetakeLastGoal();
  this.showNotification("Last goal removed", "success");
};

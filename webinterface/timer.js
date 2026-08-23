/* global ApplicationClient */

ApplicationClient.prototype.setupTimerHandlers = function () {
  const startBtn = document.getElementById("startTimerBtn");
  if (!startBtn) {
    return;
  }

  startBtn.addEventListener("click", () => {
    console.log("[UI] Start Timer clicked");
    if (this.wsClient.isConnected()) {
      this.wsClient.sendStartTimer();
      this.timerRunning = true;
      this.updateStartTimerButtonState();
    } else {
      this.showNotification("Not connected to server", "error");
    }
  });

  this.updateStartTimerButtonState();
};

ApplicationClient.prototype.syncClockWithServer = function (
  showNotification = false,
) {
  if (!this.wsClient.isConnected()) {
    if (showNotification) {
      this.showNotification("Not connected to server", "error");
    }
    return false;
  }

  const now = new Date();
  const sent = this.wsClient.sendSetClockTime(now.getTime(), now.toISOString());
  if (!sent) {
    if (showNotification) {
      this.showNotification("Clock sync failed", "error");
    }
    return false;
  }

  this.wsClient.requestClockTime();
  if (showNotification) {
    this.showNotification("Clock synchronized", "success");
  }
  return true;
};

ApplicationClient.prototype.updateScoreTimeDisplay = function (
  home,
  away,
  time,
) {
  const homeEl = document.getElementById("homeScore");
  const awayEl = document.getElementById("awayScore");
  const timeEl = document.getElementById("matchTime");

  if (homeEl) homeEl.textContent = String(home ?? 0);
  if (awayEl) awayEl.textContent = String(away ?? 0);
  if (timeEl) timeEl.textContent = time || "00:00";
};

ApplicationClient.prototype.updateStartTimerButtonState = function () {
  const startBtn = document.getElementById("startTimerBtn");
  if (!startBtn) {
    return;
  }

  const enabled =
    (this.currentMatchState === 1 || this.currentMatchState === 3) &&
    !this.timerRunning;
  startBtn.disabled = !enabled;
};

ApplicationClient.prototype.updateClockTimeDisplay = function (time, epochMs) {
  const clockEl = document.getElementById("systemClockTime");
  const epochEl = document.getElementById("systemClockEpoch");

  if (clockEl) {
    clockEl.textContent = time || "00:00:00";
  }

  if (epochEl) {
    const numericEpoch = Number(epochMs);
    epochEl.textContent = Number.isFinite(numericEpoch)
      ? String(numericEpoch)
      : "-";
  }
};

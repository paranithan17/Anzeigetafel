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

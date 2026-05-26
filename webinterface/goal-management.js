/* global ApplicationClient */

ApplicationClient.prototype.setupGoalHandlers = function () {
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
};

ApplicationClient.prototype.populateGoalSelectionModal = function () {
  const homeList = document.getElementById("goalHomeList");
  const awayList = document.getElementById("goalAwayList");
  if (!homeList || !awayList) {
    return;
  }

  const renderList = (element, team) => {
    const players = this.players[team] || [];
    if (!players || players.length === 0) {
      element.innerHTML = '<p class="empty-list-message">No players</p>';
      return;
    }

    let html = "";
    for (const player of players) {
      html += `<div class="goal-list-item">
          <span class="goal-list-item-name"><strong>${player.number}</strong> ${player.name}</span>
          <button class="scorer-btn scorer-button" data-team="${team}" data-number="${player.number}" data-name="${player.name}">Select</button>
        </div>`;
    }
    element.innerHTML = html;

    const buttons = element.querySelectorAll(".scorer-btn");
    buttons.forEach((btn) => {
      btn.addEventListener("click", (e) => {
        const teamName = e.currentTarget.dataset.team;
        const number = parseInt(e.currentTarget.dataset.number, 10);
        const name = e.currentTarget.dataset.name;

        if (teamName !== this.requestingTeam) {
          this.pendingGoalData = { team: teamName, number, name };
          this.showOwnGoalConfirmation(
            `${name} (#${number}) is from the opponent team. Mark as own goal?`,
          );
        } else {
          this.completeGoalEntry({
            team: teamName,
            number,
            name,
            isOwnGoal: false,
          });
        }
      });
    });
  };

  renderList(homeList, "Home");
  renderList(awayList, "Away");
};

ApplicationClient.prototype.showGoalSelector = function (requestingTeam) {
  this.requestingTeam = requestingTeam;

  const timeInput = document.getElementById("goalMinuteInput");
  if (timeInput) {
    timeInput.value = String(this.getSuggestedGoalMinute());
  }

  this.populateGoalSelectionModal();
  this.showModal("goalSelectModal");

  const closeBtn = document.getElementById("goalModalClose");
  if (closeBtn) {
    closeBtn.onclick = () => {
      this.hideModal("goalSelectModal");
      this.requestingTeam = null;
    };
  }

  const cancelBtn = document.getElementById("goalCancelBtn");
  if (cancelBtn) {
    cancelBtn.onclick = () => {
      this.hideModal("goalSelectModal");
      this.requestingTeam = null;
    };
  }
};

ApplicationClient.prototype.showOwnGoalConfirmation = function (message) {
  const textElement = document.getElementById("ownGoalConfirmText");
  if (textElement) {
    textElement.textContent = message;
  }

  this.showModal("ownGoalConfirmModal");

  const yesBtn = document.getElementById("ownGoalConfirmYesBtn");
  if (yesBtn) {
    yesBtn.onclick = () => {
      this.hideModal("ownGoalConfirmModal");
      if (this.pendingGoalData) {
        this.completeGoalEntry({
          ...this.pendingGoalData,
          isOwnGoal: true,
        });
      }
    };
  }

  const noBtn = document.getElementById("ownGoalConfirmNoBtn");
  if (noBtn) {
    noBtn.onclick = () => {
      this.hideModal("ownGoalConfirmModal");
      if (this.pendingGoalData) {
        this.completeGoalEntry({
          ...this.pendingGoalData,
          isOwnGoal: false,
        });
      }
    };
  }

  const cancelBtn = document.getElementById("ownGoalConfirmCancelBtn");
  if (cancelBtn) {
    cancelBtn.onclick = () => {
      this.hideModal("ownGoalConfirmModal");
      this.pendingGoalData = null;
    };
  }

  const closeBtn = document.getElementById("ownGoalConfirmModalClose");
  if (closeBtn) {
    closeBtn.onclick = () => {
      this.hideModal("ownGoalConfirmModal");
      this.pendingGoalData = null;
    };
  }
};

ApplicationClient.prototype.completeGoalEntry = function (goalData) {
  const timeInput = document.getElementById("goalMinuteInput");
  let goalMinute = this.getSuggestedGoalMinute();
  if (timeInput && timeInput.value) {
    goalMinute = parseInt(timeInput.value, 10);
  }

  this.sendGoalWithPlayer(goalData.team, {
    number: goalData.number,
    name: goalData.name,
    goalMinute,
    isOwnGoal: goalData.isOwnGoal,
  });

  this.hideModal("goalSelectModal");
  this.requestingTeam = null;
  this.pendingGoalData = null;
};

ApplicationClient.prototype.sendGoalWithPlayer = function (team, player) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  const goalMinute = player.goalMinute !== undefined ? player.goalMinute : 0;
  const isOwnGoal = player.isOwnGoal !== undefined ? player.isOwnGoal : false;

  const success = this.wsClient.sendGoal(
    team,
    player.number,
    player.name,
    goalMinute,
    isOwnGoal,
  );

  if (success) {
    let message = `${player.name} (#${player.number}) recorded for ${team}`;
    if (isOwnGoal) {
      message += " - OWN GOAL";
    }
    this.showNotification(message, "success");
  } else {
    this.showNotification("Failed to send goal", "error");
  }

  return success;
};

ApplicationClient.prototype.getSuggestedGoalMinute = function () {
  const timeEl = document.getElementById("matchTime");
  const timeText = timeEl ? String(timeEl.textContent || "").trim() : "";
  const minutesText = timeText.split(":")[0];
  const minutes = parseInt(minutesText, 10);

  if (Number.isFinite(minutes) && minutes >= 0) {
    return minutes + 1;
  }

  return 1;
};

/* global ApplicationClient */

ApplicationClient.prototype.setupMatchStateHandlers = function () {
  const form = document.getElementById("matchStateForm");
  if (form) {
    form.addEventListener("submit", (e) => {
      e.preventDefault();
      this.handleFormSubmit();
    });
  }

  const radioButtons = document.querySelectorAll('input[name="matchState"]');
  radioButtons.forEach((radio) => {
    radio.addEventListener("change", (e) => {
      console.log(`[UI] Radio selected: ${e.target.value}`);
    });
  });
};

ApplicationClient.prototype.handleMatchStateUpdate = function (
  stateId,
  stateName,
) {
  console.log(`[State Update] State changed to ${stateName} (${stateId})`);

  this.currentMatchState = stateId;

  const radioId = this.stateMap[stateId];
  if (radioId) {
    const radioElement = document.getElementById(radioId);
    if (radioElement) {
      radioElement.checked = true;
      console.log(`[UI] Updated radio button to: ${radioId}`);
    }
  }

  this.updateStateDisplay(stateId, stateName);
  this.updateStartTimerButtonState();
};

ApplicationClient.prototype.updateStateDisplay = function (stateId, stateName) {
  const stateDisplay = document.getElementById("currentStateDisplay");
  if (stateDisplay) {
    stateDisplay.textContent =
      stateName || this.stateNameMap[stateId] || "Unknown";
    stateDisplay.className = `state-display state-${stateId}`;
  }
};

ApplicationClient.prototype.handleFormSubmit = function () {
  const selectedRadio = document.querySelector(
    'input[name="matchState"]:checked',
  );

  if (!selectedRadio) {
    this.showNotification("Please select a match state", "warning");
    return;
  }

  const value = selectedRadio.value;
  let stateId = null;

  for (const [id, name] of Object.entries(this.stateMap)) {
    if (name === value) {
      stateId = parseInt(id, 10);
      break;
    }
  }

  if (stateId === null) {
    this.showNotification("Invalid state selected", "error");
    return;
  }

  if (this.timerRunning && stateId !== this.currentMatchState) {
    this.showMatchStateConfirmation(stateId);
    return;
  }

  const currentStateIsActive =
    this.currentMatchState === 1 || this.currentMatchState === 3;
  const newStateIsActive = stateId === 1 || stateId === 3;

  if (currentStateIsActive && !newStateIsActive) {
    this.showMatchStateConfirmation(stateId);
    return;
  }

  console.log(`[Application] Submitting state change: ${value} (${stateId})`);
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return;
  }

  this.wsClient.sendMatchStateChange(stateId);
};

ApplicationClient.prototype.showMatchStateConfirmation = function (stateId) {
  const textElement = document.getElementById("matchStateConfirmText");
  if (textElement) {
    textElement.textContent = `Are you sure you want to change to ${this.stateNameMap[stateId]}? This will stop the current match.`;
  }

  this.showModal("matchStateConfirmModal");

  const yesBtn = document.getElementById("matchStateConfirmYesBtn");
  if (yesBtn) {
    yesBtn.onclick = () => {
      this.hideModal("matchStateConfirmModal");
      if (!this.wsClient.isConnected()) {
        this.showNotification("Not connected to server", "error");
        return;
      }

      this.timerRunning = false;
      this.updateStartTimerButtonState();
      this.wsClient.sendMatchStateChange(stateId);
    };
  }

  const noBtn = document.getElementById("matchStateConfirmNoBtn");
  if (noBtn) {
    noBtn.onclick = () => {
      this.hideModal("matchStateConfirmModal");
      const radioId = this.stateMap[this.currentMatchState];
      if (radioId) {
        const radioElement = document.getElementById(radioId);
        if (radioElement) {
          radioElement.checked = true;
        }
      }
    };
  }

  const closeBtn = document.getElementById("matchStateConfirmModalClose");
  if (closeBtn) {
    closeBtn.onclick = () => {
      this.hideModal("matchStateConfirmModal");
      const radioId = this.stateMap[this.currentMatchState];
      if (radioId) {
        const radioElement = document.getElementById(radioId);
        if (radioElement) {
          radioElement.checked = true;
        }
      }
    };
  }
};

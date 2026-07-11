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

  const matchStateSlidesButtons = document.querySelectorAll(
    ".matchstate-files-btn[data-state-key]",
  );
  matchStateSlidesButtons.forEach((button) => {
    button.addEventListener("click", () => {
      this.openMatchStateSlidesModal(button.dataset.stateKey);
    });
  });

  const matchStateSlidesModalClose = document.getElementById(
    "matchStateSlidesModalClose",
  );
  if (matchStateSlidesModalClose) {
    matchStateSlidesModalClose.addEventListener("click", () => {
      this.hideModal("matchStateSlidesModal");
    });
  }

  const matchStateSlidesCancelBtn = document.getElementById(
    "matchStateSlidesCancelBtn",
  );
  if (matchStateSlidesCancelBtn) {
    matchStateSlidesCancelBtn.addEventListener("click", () => {
      this.hideModal("matchStateSlidesModal");
    });
  }

  const matchStateSlidesUploadBtn = document.getElementById(
    "matchStateSlidesUploadBtn",
  );
  const matchStateSlidesUploadInput = document.getElementById(
    "matchStateSlidesUploadInput",
  );

  if (matchStateSlidesUploadBtn && matchStateSlidesUploadInput) {
    matchStateSlidesUploadBtn.addEventListener("click", () => {
      matchStateSlidesUploadInput.click();
    });

    matchStateSlidesUploadInput.addEventListener("change", (e) => {
      this.handleMatchStateSlidesUpload(e.target.files);
      matchStateSlidesUploadInput.value = "";
    });
  }
};

ApplicationClient.prototype.openMatchStateSlidesModal = function (stateKey) {
  if (!stateKey) {
    this.showNotification("No slides state selected", "warning");
    return;
  }

  this.currentSlidesStateKey = stateKey;

  const title = document.getElementById("matchStateSlidesModalTitle");
  if (title) {
    title.textContent = `Manage Slides - ${stateKey}`;
  }

  this.showModal("matchStateSlidesModal");
  this.requestMatchStateSlides(stateKey);
};

ApplicationClient.prototype.requestMatchStateSlides = function (stateKey) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.requestMatchStateSlides(stateKey);
};

ApplicationClient.prototype.displayMatchStateSlides = function (
  stateKey,
  directory,
  entries,
) {
  if (stateKey !== this.currentSlidesStateKey) {
    return;
  }

  const listElement = document.getElementById("matchStateSlidesList");
  if (!listElement) {
    return;
  }

  if (!entries || entries.length === 0) {
    listElement.innerHTML =
      '<p class="empty-list-message">No files or directories found</p>';
    return;
  }

  let html = "";
  for (const entry of entries) {
    const entryLabel = entry.isDir
      ? `[DIR] ${entry.name}`
      : `[FILE] ${entry.name}`;
    const entryClass = entry.isDir
      ? "matchstate-slides-item-name matchstate-slides-item-dir"
      : "matchstate-slides-item-name";

    html += `<div class="matchstate-slides-item" data-path="${entry.path}" data-name="${entry.name}" data-is-dir="${entry.isDir}">
      <span class="${entryClass}">${entryLabel}</span>
      <button class="matchstate-delete-btn" type="button">Delete</button>
    </div>`;
  }

  listElement.innerHTML = html;

  const deleteButtons = listElement.querySelectorAll(".matchstate-delete-btn");
  deleteButtons.forEach((button) => {
    button.addEventListener("click", (e) => {
      const item = e.target.closest(".matchstate-slides-item");
      if (!item) {
        return;
      }

      const entryPath = item.dataset.path;
      const entryName = item.dataset.name;
      const isDir = item.dataset.isDir === "true";

      if (
        window.confirm(
          `Delete ${isDir ? "directory" : "file"} \"${entryName}\" from ${directory}?`,
        )
      ) {
        this.deleteMatchStateSlide(this.currentSlidesStateKey, entryPath);
      }
    });
  });
};

ApplicationClient.prototype.deleteMatchStateSlide = function (
  stateKey,
  entryPath,
) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.deleteMatchStateSlide(stateKey, entryPath);
};

ApplicationClient.prototype.handleMatchStateSlidesUpload = function (fileList) {
  if (!fileList || fileList.length === 0) {
    return;
  }

  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return;
  }

  if (!this.currentSlidesStateKey) {
    this.showNotification("No matchstate slide folder selected", "warning");
    return;
  }

  for (const file of fileList) {
    const reader = new FileReader();
    reader.onload = (e) => {
      const dataUrl = e.target.result;
      const sent = this.wsClient.uploadMatchStateSlide(
        this.currentSlidesStateKey,
        file.name,
        dataUrl,
      );
      if (!sent) {
        this.showNotification(`Failed to upload: ${file.name}`, "error");
      }
    };
    reader.onerror = () => {
      this.showNotification(`Failed to read file: ${file.name}`, "error");
    };
    reader.readAsDataURL(file);
  }
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

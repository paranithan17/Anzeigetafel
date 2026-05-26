/* global ApplicationClient */

ApplicationClient.prototype.setupEmblemManagementHandlers = function () {
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
      if (this.currentEmblemTeam === "Home" && homeEmblemFile) {
        homeEmblemFile.click();
      } else if (this.currentEmblemTeam === "Away" && awayEmblemFile) {
        awayEmblemFile.click();
      }
    });
  }
};

ApplicationClient.prototype.openEmblemModal = function (team) {
  this.currentEmblemTeam = team;
  this.showModal("emblemSelectModal");
  this.requestSavedEmblems();
};

ApplicationClient.prototype.requestSavedEmblems = function () {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  return this.wsClient.requestSavedEmblems();
};

ApplicationClient.prototype.displaySavedEmblems = function (emblems) {
  const listElement = document.getElementById("savedEmblemsList");
  if (!listElement) {
    return;
  }

  if (!emblems || emblems.length === 0) {
    listElement.innerHTML =
      '<p class="empty-list-message">No saved emblems</p>';
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
};

ApplicationClient.prototype.sendSelectSavedEmblem = function (team, filePath) {
  if (!this.wsClient.isConnected()) {
    this.showNotification("Not connected to server", "error");
    return false;
  }

  const success = this.wsClient.sendSelectSavedEmblem(team, filePath);
  if (success) {
    this.showNotification(`${team} emblem selected`, "success");
  }

  return success;
};

ApplicationClient.prototype.handleEmblemUpload = function (file, team) {
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
};

ApplicationClient.prototype.sendEmblem = function (
  team,
  fileName,
  mimeType,
  dataUrl,
) {
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
};

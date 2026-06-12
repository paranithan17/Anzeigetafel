/**
 * @file controll_window.cpp
 * @brief GUI Implementation of the Control Window for match operator.
 *
 * Pure presentation layer. All business logic is delegated to match_controller.
 */
#include "controll_window.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

controll_window::controll_window(match_controller *controller, QWidget *parent)
    : QWidget(parent), m_controller(controller)
{
    // Team management buttons
    btnAddPlayerTeam1 = new QPushButton("+");
    btnAddPlayerTeam1->setFixedSize(30, 30);
    connect(btnAddPlayerTeam1, &QPushButton::clicked, this, &controll_window::addPlayerTeam1);

    btnAddPlayerTeam2 = new QPushButton("+");
    btnAddPlayerTeam2->setFixedSize(30, 30);
    connect(btnAddPlayerTeam2, &QPushButton::clicked, this, &controll_window::addPlayerTeam2);

    // Team lists
    listTeam1 = new QListWidget;
    listTeam2 = new QListWidget;
    connect(listTeam1, &QListWidget::itemClicked, this, &controll_window::removePlayerTeam1);
    connect(listTeam2, &QListWidget::itemClicked, this, &controll_window::removePlayerTeam2);

    // Import buttons
    btnImportTeam1 = new QPushButton("Import");
    btnImportTeam1->setFixedHeight(30);
    connect(btnImportTeam1, &QPushButton::clicked, this, &controll_window::importTeam1);

    btnImportTeam2 = new QPushButton("Import");
    btnImportTeam2->setFixedHeight(30);
    connect(btnImportTeam2, &QPushButton::clicked, this, &controll_window::importTeam2);

    // Goal buttons
    btnGoalTeam1 = new QPushButton("GOAL");
    btnGoalTeam1->setFixedHeight(50);
    QFont goalFont = btnGoalTeam1->font();
    goalFont.setPixelSize(14);
    goalFont.setBold(true);
    btnGoalTeam1->setFont(goalFont);
    connect(btnGoalTeam1, &QPushButton::clicked, this, &controll_window::addGoalTeam1);

    btnGoalTeam2 = new QPushButton("GOAL");
    btnGoalTeam2->setFixedHeight(50);
    btnGoalTeam2->setFont(goalFont);
    connect(btnGoalTeam2, &QPushButton::clicked, this, &controll_window::addGoalTeam2);

    // Emblem buttons
    btnAddEmblemTeam1 = new QPushButton("Logo Hinzufugen");
    connect(btnAddEmblemTeam1, &QPushButton::clicked, this, &controll_window::loadEmblemTeam1);

    btnAddEmblemTeam2 = new QPushButton("Logo Hinzufugen");
    connect(btnAddEmblemTeam2, &QPushButton::clicked, this, &controll_window::loadEmblemTeam2);

    // Timer button
    btnStartTimer = new QPushButton("Start Timer");
    btnStartTimer->setFixedHeight(70);
    QFont timerFont = goalFont;
    timerFont.setPixelSize(24);
    btnStartTimer->setFont(timerFont);
    btnStartTimer->setEnabled(false);
    connect(btnStartTimer, &QPushButton::clicked, this, &controll_window::startTimer);

    // Log button
    btnLog = new QPushButton("Log");
    btnLog->setFixedSize(250, 30);
    connect(btnLog, &QPushButton::clicked, this, &controll_window::log);

    // Match state selection
    QGroupBox *stateGroup = new QGroupBox("Match State");
    radioPreGame = new QRadioButton("Pre game");
    radioFirstHalf = new QRadioButton("First half");
    radioHalfTime = new QRadioButton("Half time");
    radioSecondHalf = new QRadioButton("Second half");
    radioPostGame = new QRadioButton("Post game");
    radioPreGame->setChecked(true);

    QHBoxLayout *stateLayout = new QHBoxLayout;
    stateLayout->addWidget(radioPreGame);
    stateLayout->addWidget(radioFirstHalf);
    stateLayout->addWidget(radioHalfTime);
    stateLayout->addWidget(radioSecondHalf);
    stateLayout->addWidget(radioPostGame);
    stateGroup->setLayout(stateLayout);

    btnApplyState = new QPushButton("Apply state");
    connect(btnApplyState, &QPushButton::clicked, this, &controll_window::applyStateSelection);

    // Score and time display
    score = new QLabel("0 : 0");
    score->setAlignment(Qt::AlignCenter);
    score->setStyleSheet("font-size: 36px; font-weight: bold;");

    time = new QLabel("00:00");
    time->setAlignment(Qt::AlignCenter);
    time->setStyleSheet("font-size: 18px;");

    // Build layout
    QHBoxLayout *updateTeam1 = new QHBoxLayout;
    updateTeam1->addWidget(btnAddPlayerTeam1);
    updateTeam1->addWidget(btnImportTeam1);

    QVBoxLayout *home_Team_Layout = new QVBoxLayout;
    home_Team_Layout->addWidget(new QLabel("Heim"));
    home_Team_Layout->addWidget(listTeam1);
    home_Team_Layout->addLayout(updateTeam1);
    home_Team_Layout->addWidget(btnGoalTeam1);
    home_Team_Layout->addWidget(btnAddEmblemTeam1);

    QHBoxLayout *updateTeam2 = new QHBoxLayout;
    updateTeam2->addWidget(btnAddPlayerTeam2);
    updateTeam2->addWidget(btnImportTeam2);

    QVBoxLayout *away_Team_Layout = new QVBoxLayout;
    away_Team_Layout->addWidget(new QLabel("Gaste"));
    away_Team_Layout->addWidget(listTeam2);
    away_Team_Layout->addLayout(updateTeam2);
    away_Team_Layout->addWidget(btnGoalTeam2);
    away_Team_Layout->addWidget(btnAddEmblemTeam2);

    QVBoxLayout *CenterScoreLayout = new QVBoxLayout;
    CenterScoreLayout->addWidget(score);
    CenterScoreLayout->addSpacing(-200);
    CenterScoreLayout->addWidget(time);

    QHBoxLayout *TeamLayout = new QHBoxLayout;
    TeamLayout->addItem(home_Team_Layout);
    TeamLayout->setSpacing(100);
    TeamLayout->addItem(CenterScoreLayout);
    TeamLayout->setSpacing(100);
    TeamLayout->addItem(away_Team_Layout);

    QVBoxLayout *StateLayout = new QVBoxLayout;
    StateLayout->addWidget(stateGroup);
    StateLayout->addWidget(btnApplyState);
    StateLayout->addSpacing(20);

    QVBoxLayout *centerLayout = new QVBoxLayout;
    centerLayout->addWidget(btnStartTimer);
    centerLayout->addWidget(btnLog);
    centerLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(StateLayout);
    mainLayout->addStretch(1000);
    mainLayout->addLayout(TeamLayout);
    mainLayout->addStretch(1000);
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);

    // Connect to controller signals
    if (m_controller)
    {
        connect(m_controller, &match_controller::homePlayersChanged, this, &controll_window::updateTeamList1);
        connect(m_controller, &match_controller::awayPlayersChanged, this, &controll_window::updateTeamList2);
        connect(m_controller, &match_controller::scoreChanged, this, &controll_window::updateScoreDisplay);
        connect(m_controller, &match_controller::timeChanged, this, &controll_window::updateTimeDisplay);
        connect(m_controller, &match_controller::matchStateChanged, this, &controll_window::controllerStateChanged);
        connect(m_controller, &match_controller::startTimerEnabledChanged, btnStartTimer, &QPushButton::setEnabled);
    }

    // Initial display update
    updateTeamList1();
    updateTeamList2();
    updateScoreDisplay();
}

void controll_window::setScoreboard(QWidget *board)
{
    scoreboard = board;
}

void controll_window::toggleScoreboard()
{
    if (!scoreboard)
        return;

    if (scoreboard->isVisible())
        scoreboard->hide();
    else
    {
        scoreboard->show();
        scoreboard->raise();
        scoreboard->activateWindow();
    }
}

void controll_window::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    toggleScoreboard();
}

// ============ DIALOG-BASED INPUT HANDLERS ============

void controll_window::addPlayerTeam1()
{
    if (!m_controller)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Spieler Hinzufugen");

    QVBoxLayout layout(&dialog);

    QLineEdit *numberEdit = new QLineEdit;
    numberEdit->setPlaceholderText("Spielernummer eingeben:");
    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("X. Mustermann");

    layout.addWidget(new QLabel("Spieler Nummer:"));
    layout.addWidget(numberEdit);
    layout.addWidget(new QLabel("Spieler Name:"));
    layout.addWidget(nameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    layout.addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        bool ok;
        int number = numberEdit->text().toInt(&ok);
        QString name = nameEdit->text();

        if (ok && !name.isEmpty())
        {
            m_controller->addPlayer(match_controller::TeamSide::Home, number, name);
        }
        else
        {
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid number and name.");
        }
    }
}

void controll_window::addPlayerTeam2()
{
    if (!m_controller)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Spieler Hinzufugen");

    QVBoxLayout layout(&dialog);

    QLineEdit *numberEdit = new QLineEdit;
    numberEdit->setPlaceholderText("Spielernummer eingeben:");
    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("X. Mustermann");

    layout.addWidget(new QLabel("Spieler Nummer:"));
    layout.addWidget(numberEdit);
    layout.addWidget(new QLabel("Spieler Name:"));
    layout.addWidget(nameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    layout.addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        bool ok;
        int number = numberEdit->text().toInt(&ok);
        QString name = nameEdit->text();

        if (ok && !name.isEmpty())
        {
            m_controller->addPlayer(match_controller::TeamSide::Away, number, name);
        }
        else
        {
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid number and name.");
        }
    }
}

void controll_window::removePlayerTeam1(QListWidgetItem *item)
{
    if (!m_controller || !item)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Spieler bearbeiten");

    QVBoxLayout layout(&dialog);
    QLabel *info = new QLabel("Willst Du den Spieler entfernen?");
    layout.addWidget(info);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QPushButton *removeButton = new QPushButton("Entfernen");
    buttonBox->addButton(removeButton, QDialogButtonBox::AcceptRole);
    layout.addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(removeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted)
    {
        bool ok;
        int number = item->text().split(" ")[0].toInt(&ok);
        if (ok)
        {
            m_controller->removePlayer(match_controller::TeamSide::Home, number);
        }
    }
}

void controll_window::removePlayerTeam2(QListWidgetItem *item)
{
    if (!m_controller || !item)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Spieler bearbeiten");

    QVBoxLayout layout(&dialog);
    QLabel *info = new QLabel("Willst Du den Spieler entfernen?");
    layout.addWidget(info);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QPushButton *removeButton = new QPushButton("Entfernen");
    buttonBox->addButton(removeButton, QDialogButtonBox::AcceptRole);
    layout.addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(removeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted)
    {
        bool ok;
        int number = item->text().split(" ")[0].toInt(&ok);
        if (ok)
        {
            m_controller->removePlayer(match_controller::TeamSide::Away, number);
        }
    }
}

void controll_window::addGoalTeam1()
{
    if (!m_controller)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Tor Heimmannschaft");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QListWidget *homePlayers = new QListWidget;
    QListWidget *awayPlayers = new QListWidget;

    for (const auto &player : m_controller->getHomePlayers())
        homePlayers->addItem(QString::number(player->getNumber()) + " - " + player->getName());
    for (const auto &player : m_controller->getAwayPlayers())
        awayPlayers->addItem(QString::number(player->getNumber()) + " - " + player->getName());

    QHBoxLayout *listsLayout = new QHBoxLayout;
    listsLayout->addWidget(homePlayers);
    listsLayout->addWidget(awayPlayers);
    mainLayout->addLayout(listsLayout);

    QLineEdit *timeEdit = new QLineEdit;
    QLineEdit *playerEdit = new QLineEdit;
    playerEdit->setReadOnly(true);
    timeEdit->setText(QString::number(m_controller->suggestedGoalMinute()));

    QHBoxLayout *fieldsLayout = new QHBoxLayout;
    fieldsLayout->addWidget(new QLabel("Zeit (min):"));
    fieldsLayout->addWidget(timeEdit);
    fieldsLayout->addWidget(new QLabel("Spieler:"));
    fieldsLayout->addWidget(playerEdit);
    mainLayout->addLayout(fieldsLayout);

    QString selectedTeam;
    connect(homePlayers, &QListWidget::itemClicked, [&](QListWidgetItem *item)
            {
                playerEdit->setText(item->text());
                selectedTeam = "Home"; });
    connect(awayPlayers, &QListWidget::itemClicked, [&](QListWidgetItem *item)
            {
                playerEdit->setText(item->text());
                selectedTeam = "Away"; });

    QPushButton *saveButton = new QPushButton("Save");
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, [&]()
            {
                bool ok;
                int minute = timeEdit->text().toInt(&ok);
                if (!ok)
                {
                    QMessageBox::warning(&dialog, "Error", "Invalid time");
                    return;
                }

                const bool hasPlayer = !playerEdit->text().trimmed().isEmpty();
                int number = -1;
                QString name;
                match_controller::TeamSide scorerTeam = match_controller::TeamSide::Home;
                bool isOwnGoal = false;

                if (hasPlayer)
                {
                    QString text = playerEdit->text();
                    number = text.split(" ")[0].toInt(&ok);
                    name = text.mid(text.indexOf('-') + 2);
                    if (!ok || name.isEmpty())
                    {
                        QMessageBox::warning(&dialog, "Error", "Invalid player selection");
                        return;
                    }

                    scorerTeam = selectedTeam == "Home" ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
                    isOwnGoal = selectedTeam == "Away";
                }

                match_controller::GoalData goalData{
                    scorerTeam,
                    number,
                    name,
                    static_cast<unsigned>(minute),
                    isOwnGoal
                };

                m_controller->addGoalWithValidation(goalData);
                dialog.accept(); });

    dialog.exec();
}

void controll_window::addGoalTeam2()
{
    if (!m_controller)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Tor Gastmannschaft");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QListWidget *homePlayers = new QListWidget;
    QListWidget *awayPlayers = new QListWidget;

    for (const auto &player : m_controller->getHomePlayers())
        homePlayers->addItem(QString::number(player->getNumber()) + " - " + player->getName());
    for (const auto &player : m_controller->getAwayPlayers())
        awayPlayers->addItem(QString::number(player->getNumber()) + " - " + player->getName());

    QHBoxLayout *listsLayout = new QHBoxLayout;
    listsLayout->addWidget(homePlayers);
    listsLayout->addWidget(awayPlayers);
    mainLayout->addLayout(listsLayout);

    QLineEdit *timeEdit = new QLineEdit;
    QLineEdit *playerEdit = new QLineEdit;
    playerEdit->setReadOnly(true);
    timeEdit->setText(QString::number(m_controller->suggestedGoalMinute()));

    QHBoxLayout *fieldsLayout = new QHBoxLayout;
    fieldsLayout->addWidget(new QLabel("Zeit (min):"));
    fieldsLayout->addWidget(timeEdit);
    fieldsLayout->addWidget(new QLabel("Spieler:"));
    fieldsLayout->addWidget(playerEdit);
    mainLayout->addLayout(fieldsLayout);

    QString selectedTeam;
    connect(homePlayers, &QListWidget::itemClicked, [&](QListWidgetItem *item)
            {
                playerEdit->setText(item->text());
                selectedTeam = "Home"; });
    connect(awayPlayers, &QListWidget::itemClicked, [&](QListWidgetItem *item)
            {
                playerEdit->setText(item->text());
                selectedTeam = "Away"; });

    QPushButton *saveButton = new QPushButton("Save");
    QPushButton *cancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, [&]()
            {
                bool ok;
                int minute = timeEdit->text().toInt(&ok);
                if (!ok)
                {
                    QMessageBox::warning(&dialog, "Error", "Invalid time");
                    return;
                }

                const bool hasPlayer = !playerEdit->text().trimmed().isEmpty();
                int number = -1;
                QString name;
                match_controller::TeamSide scorerTeam = match_controller::TeamSide::Away;
                bool isOwnGoal = false;

                if (hasPlayer)
                {
                    QString text = playerEdit->text();
                    number = text.split(" ")[0].toInt(&ok);
                    name = text.mid(text.indexOf('-') + 2);
                    if (!ok || name.isEmpty())
                    {
                        QMessageBox::warning(&dialog, "Error", "Invalid player selection");
                        return;
                    }

                    scorerTeam = selectedTeam == "Home" ? match_controller::TeamSide::Home : match_controller::TeamSide::Away;
                    isOwnGoal = selectedTeam == "Home";
                }

                match_controller::GoalData goalData{
                    scorerTeam,
                    number,
                    name,
                    static_cast<unsigned>(minute),
                    isOwnGoal
                };

                m_controller->addGoalWithValidation(goalData);
                dialog.accept(); });

    dialog.exec();
}

void controll_window::startTimer()
{
    if (!m_controller)
        return;

    m_controller->requestTimerStart();
}

void controll_window::log()
{
    if (!m_controller)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Log window");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QHBoxLayout *logButtonLayout = new QHBoxLayout;
    QPushButton *restartButton = new QPushButton("Restart");
    QPushButton *resetButton = new QPushButton("Reset");
    QPushButton *undoGoalButton = new QPushButton("Tor zurucknehmen");
    logButtonLayout->addWidget(restartButton);
    logButtonLayout->addWidget(resetButton);
    logButtonLayout->addWidget(undoGoalButton);

    QPushButton *cancelButton = new QPushButton("Cancel");
    cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addLayout(logButtonLayout);
    layout->addWidget(cancelButton);

    connect(restartButton, &QPushButton::clicked, [&]()
            {
                m_controller->requestTimerRestart();
                dialog.accept(); });

    connect(resetButton, &QPushButton::clicked, [&]()
            {
                m_controller->requestMatchReset();
                dialog.accept(); });

    connect(undoGoalButton, &QPushButton::clicked, [&]()
            {
                m_controller->removeLastGoal();
                dialog.accept(); });

    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void controll_window::loadEmblemTeam1()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Vereinslogo einfugen", "", "Images (*.png *.jpg *.bmp)");
    if (!filePath.isEmpty())
    {
        if (m_controller)
        {
            m_controller->setTeamEmblem(match_controller::TeamSide::Home, filePath);
        }
        emit emblemChanged("Home", filePath);
    }
}

void controll_window::loadEmblemTeam2()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Vereinslogo einfugen", "", "Images (*.png *.jpg *.bmp)");
    if (!filePath.isEmpty())
    {
        if (m_controller)
        {
            m_controller->setTeamEmblem(match_controller::TeamSide::Away, filePath);
        }
        emit emblemChanged("Away", filePath);
    }
}

void controll_window::importTeam1()
{
    if (!m_controller)
        return;

    const QString fileName = QFileDialog::getOpenFileName(this, "Import Player List (Home Team)", "", "CSV Files (*.csv *.CSV);; All Files(*)");
    if (fileName.isEmpty())
        return;

    int importedCount = 0;
    if (!m_controller->importPlayersFromCsv(match_controller::TeamSide::Home, fileName, &importedCount))
    {
        QMessageBox::warning(this, "Import Error", "Could not import players from file");
    }
}

void controll_window::importTeam2()
{
    if (!m_controller)
        return;

    const QString fileName = QFileDialog::getOpenFileName(this, "Import Player List (Away Team)", "", "CSV Files (*.csv *.CSV);; All Files(*)");
    if (fileName.isEmpty())
        return;

    int importedCount = 0;
    if (!m_controller->importPlayersFromCsv(match_controller::TeamSide::Away, fileName, &importedCount))
    {
        QMessageBox::warning(this, "Import Error", "Could not import players from file");
    }
}

void controll_window::applyStateSelection()
{
    if (!m_controller)
        return;

    match_controller::MatchState targetState = selectedState();
    bool changed = m_controller->requestStateChange(targetState, false);

    if (!changed)
    {
        // Timer is running - confirm state change
        int ret = QMessageBox::question(this, "Confirm State Change",
                                        "Timer is running. Stop timer and change state?",
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            m_controller->requestStateChange(targetState, true);
        }
        else
        {
            // Restore radio button to previous state
            setStateSelection(static_cast<match_controller::MatchState>(m_controller->currentState()));
        }
    }
}

// ============ DISPLAY UPDATE SLOTS ============

void controll_window::updateScoreDisplay()
{
    if (!m_controller)
        return;

    score->setText(QString::number(m_controller->getHomeScore()) + " : " + QString::number(m_controller->getAwayScore()));
}

void controll_window::updateTimeDisplay(const QString &elapsedTime)
{
    time->setText(elapsedTime);
}

void controll_window::updateTeamList1()
{
    listTeam1->clear();
    if (!m_controller)
        return;

    for (const auto &player : m_controller->getHomePlayers())
    {
        listTeam1->addItem(QString::number(player->getNumber()) + " - " + player->getName());
    }
}

void controll_window::updateTeamList2()
{
    listTeam2->clear();
    if (!m_controller)
        return;

    for (const auto &player : m_controller->getAwayPlayers())
    {
        listTeam2->addItem(QString::number(player->getNumber()) + " - " + player->getName());
    }
}

void controll_window::controllerStateChanged(int state)
{
    setStateSelection(static_cast<match_controller::MatchState>(state));
}

// ============ HELPER FUNCTIONS ============

match_controller::MatchState controll_window::selectedState() const
{
    if (radioFirstHalf->isChecked())
        return match_controller::MatchState::FirstHalf;
    if (radioHalfTime->isChecked())
        return match_controller::MatchState::HalfTime;
    if (radioSecondHalf->isChecked())
        return match_controller::MatchState::SecondHalf;
    if (radioPostGame->isChecked())
        return match_controller::MatchState::PostGame;

    return match_controller::MatchState::PreGame;
}

void controll_window::setStateSelection(match_controller::MatchState state)
{
    switch (state)
    {
    case match_controller::MatchState::PreGame:
        radioPreGame->setChecked(true);
        break;
    case match_controller::MatchState::FirstHalf:
        radioFirstHalf->setChecked(true);
        break;
    case match_controller::MatchState::HalfTime:
        radioHalfTime->setChecked(true);
        break;
    case match_controller::MatchState::SecondHalf:
        radioSecondHalf->setChecked(true);
        break;
    case match_controller::MatchState::PostGame:
        radioPostGame->setChecked(true);
        break;
    }
}

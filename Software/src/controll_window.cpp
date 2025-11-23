/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  controll_window.cpp
 * @class controll_window
 * @brief
 * This is the class for the controll-element. It includes
 * the management of the players, management of the goal,
 * start and restart options of the timer and log-setup.
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 27.05.2025
 * @version V1.1, 01.06.2025 changed std:string to QString
 * @version V1.2, 01.06.2025 Detecting own goals
 * @version V1.3, 02.06.2025 load the emblems of the clubs
 * @version V1.4, 02.06.2025 import player list as a csv-file
 * @version V2.0, 07.06.2025 timer first and second period 00:00 - 45:00 and 45:00 - 90:00
 *
 *
 * @note This code has been created with help of chatgpt
 * @note The extras of the GUI rather the minimum requirements are marked with a comment block
 */
#include "controll_window.h"
#include "QMessageBox"

controll_window::controll_window(QWidget *parent) : QWidget(parent)
{
    /**
     * @brief Managing and displaying (in controll window) the playerlist for both teams
     */

    // list
    listTeam1 = new QListWidget;
    listTeam2 = new QListWidget;

    // Add player for home Team
    QPushButton *btnAddPlayerTeam1 = new QPushButton("+");
    btnAddPlayerTeam1->setFixedSize(30, 30);
    connect(btnAddPlayerTeam1, &QPushButton::clicked, this, &controll_window::AddPlayerTeam1);

    // connect remove home player
    connect(listTeam1, &QListWidget::itemClicked, this, &controll_window::RemovePlayerTeam1);

    // Add player for away Team
    QPushButton *btnAddPlayerTeam2 = new QPushButton("+");
    btnAddPlayerTeam2->setFixedSize(30, 30);
    connect(btnAddPlayerTeam2, &QPushButton::clicked, this, &controll_window::AddPlayerTeam2);

    // connect remove away player
    connect(listTeam2, &QListWidget::itemClicked, this, &controll_window::RemovePlayerTeam2);

    /**********************************/
    // Import teams via csv file
    QPushButton *btnImportTeam1 = new QPushButton("Import");
    btnImportTeam1->setFixedHeight(30);
    connect(btnImportTeam1, &QPushButton::clicked, this, &controll_window::ImportTeam1);

    QPushButton *btnImportTeam2 = new QPushButton("Import");
    btnImportTeam2->setFixedHeight(30);
    connect(btnImportTeam2, &QPushButton::clicked, this, &controll_window::ImportTeam2);
    /**********************************/

    /**
     * @brief Managing goal
     */

    btnGoalTeam1 = new QPushButton("GOAL");
    btnGoalTeam1->setFixedHeight(50); // here to change the size of the btn
    // font settings for both buttons
    QFont goalFont = btnGoalTeam1->font();
    goalFont.setPixelSize(14); // here to change the size of letters
    goalFont.setBold(true);
    btnGoalTeam1->setFont(goalFont);
    connect(btnGoalTeam1, &QPushButton::clicked, this, &controll_window::AddGoalTeam1);

    btnGoalTeam2 = new QPushButton("GOAL");
    btnGoalTeam2->setFixedHeight(50); // here to change the size of the btn
    btnGoalTeam2->setFont(goalFont);
    connect(btnGoalTeam2, &QPushButton::clicked, this, &controll_window::AddGoalTeam2);

    // Transmits the date to score_memory
    ScoreMemory = new score_memory(this);

    /**********************************/
    /**
     * @brief Adding emblem buttons
     */
    btnAddEmblemTeam1 = new QPushButton("Logo Hinzufügen");
    connect(btnAddEmblemTeam1, &QPushButton::clicked, this, &controll_window::loadEmblemTeam1);

    btnAddEmblemTeam2 = new QPushButton("Logo Hinzufügen");
    connect(btnAddEmblemTeam2, &QPushButton::clicked, this, &controll_window::loadEmblemTeam2);
    /**********************************/

    /**
     * @brief Timer and log buttons
     */

    btnStartTimer = new QPushButton("Start Timer");
    btnStartTimer->setFixedHeight(70);
    QFont timerFont = btnGoalTeam1->font();
    timerFont.setPixelSize(24); // here to change the size of letters
    timerFont.setBold(true);
    btnStartTimer->setFont(timerFont);
    connect(btnStartTimer, &QPushButton::clicked, this, &controll_window::StartTime);

    btnLog = new QPushButton("Log");
    btnLog->setFixedSize(250, 30);
    connect(btnLog, &QPushButton::clicked, this, &controll_window::Log);

    /**********************************/
    /**
     * @brief Match state (simple state machine)
     * @date 23.11.2025
     */
    QGroupBox *stateGroup = new QGroupBox("Match State");

    radioPreGame   = new QRadioButton("PreGame");
    radioFirstHalf = new QRadioButton("First half");
    radioHalfTime  = new QRadioButton("Half time");
    radioSecondHalf= new QRadioButton("Second half");
    radioPostGame  = new QRadioButton("Post game");

    // Default state when the application starts
    radioPreGame->setChecked(true);
    m_currentState = MatchState::PreGame;

    QHBoxLayout *stateLayout = new QHBoxLayout;
    stateLayout->addWidget(radioPreGame);
    stateLayout->addWidget(radioFirstHalf);
    stateLayout->addWidget(radioHalfTime);
    stateLayout->addWidget(radioSecondHalf);
    stateLayout->addWidget(radioPostGame);
    stateGroup->setLayout(stateLayout);

    btnApplyState = new QPushButton("Apply state");
    connect(btnApplyState, &QPushButton::clicked,
            this, &controll_window::applyStateSelection);


    /**
     * @brief Operator Score and Time display
     */
    score = new QLabel("0 : 0");
    score->setAlignment(Qt::AlignCenter);
    score->setStyleSheet("font-size: 36px; font-weight: bold;");
    connect(ScoreMemory, &score_memory::goalsUpdated, this, &controll_window::UpdateScoreDisplay);

    time = new QLabel("00:00");
    time->setAlignment(Qt::AlignCenter);
    time->setStyleSheet("font-size: 18px;");

    /**
     * @brief Managing Layout
     */
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
    away_Team_Layout->addWidget(new QLabel("Gäste"));
    away_Team_Layout->addWidget(listTeam2);
    away_Team_Layout->addLayout(updateTeam2);
    away_Team_Layout->addWidget(btnGoalTeam2);
    away_Team_Layout->addWidget(btnAddEmblemTeam2);

    QVBoxLayout *CenterScoreLayout = new QVBoxLayout;
    CenterScoreLayout->addWidget(score);
    CenterScoreLayout->addSpacing(-200); // Pulling the timer upper to score
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
    mainLayout->addLayout(TeamLayout);
    mainLayout->addStretch(1000); // Add vertical space
    mainLayout->addLayout(StateLayout);
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);
}

void controll_window::updateTeamList1()
{
    /**
     * @brief udates the the player list after adding and removing a player in team1 (home)
     */
    listTeam1->clear();
    for (const auto &player : team1.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam1->addItem(item);
    }
}

void controll_window::updateTeamList2()
{
    /**
     * @brief udates the the player list after adding and removing a player in team2 (away)
     */
    listTeam2->clear();
    for (const auto &player : team2.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam2->addItem(item);
    }
}

void controll_window::AddPlayerTeam1()
{
    /**
     * @brief when the plus button has been clicked, it pop ups a dialog,
     *  where the name and number of a player can be entered. This initiales can be saved in the list
     *
     * @note this part of the code was coded with help from chatgpt.
     *
     */
    QDialog dialog(this);
    dialog.setWindowTitle("Spieler Hinzufügen");

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
        unsigned number = numberEdit->text().toInt(&ok);
        QString name = nameEdit->text();

        if (ok && !name.isEmpty())
        {
            team1.addPlayer(number, name);
            updateTeamList1();
        }
        else
        {
            QMessageBox::warning(this, "Input Error", "Bitte eine gültigen Nummer und Namen eingeben!");
        }
    }
}

void controll_window::AddPlayerTeam2()
{
    /**
     * @brief when the plus button has been clicked, it pop ups a dialog,
     *  where the name and number of a player can be entered. This initiales can be saved in the list
     *
     * @note this part of the code was coded with help from chatgpt.
     *
     */
    QDialog dialog(this);
    dialog.setWindowTitle("Spieler Hinzufügen");

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
        unsigned number = numberEdit->text().toInt(&ok);
        QString name = nameEdit->text();

        if (ok && !name.isEmpty())
        {
            team2.addPlayer(number, name);
            updateTeamList2();
        }
        else
        {
            QMessageBox::warning(this, "Input Error", "Bitte eine gültigen Nummer und Namen eingeben!");
        }
    }
}

void controll_window::RemovePlayerTeam1(QListWidgetItem *item)
{
    /**
     * @brief when a player in the list of the controll window is clicked, its pops up a dialog
     * window, where the chosen player can be removed from the list.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
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
        QString text = item->text();
        int number = text.section(" - ", 0, 0).toInt();
        team1.removePlayer(number);
        updateTeamList1();
    }
}

void controll_window::RemovePlayerTeam2(QListWidgetItem *item)
{
    /**
     * @brief when a player in the list of the controll window is clicked, its pops up a dialog
     * window, where the chosen player can be removed from the list.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
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
        QString text = item->text();
        int number = text.section(" - ", 0, 0).toInt();
        team2.removePlayer(number);
        updateTeamList2();
    }
}

void controll_window::AddGoalTeam1()
{
    /**
     * @brief By pressing the GOAL button, it pos up a new dialog window where a goal for the
     * team1 can be added. It automatically enters the time when the goal button was pressed.
     * But the time is adjustable. By chosing the player from the opponents lists,
     * it asks if it was a own goal. If it is a own goal, it will also be marked as one.
     * The time and scorer are returned to the score_memory class.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    qDebug() << "Goal for home Team!";
    QDialog dialog(this);
    dialog.setWindowTitle("Tor Heimmannschaft");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    // --- Player Lists ---
    QListWidget *listTeam1 = new QListWidget;
    QListWidget *listTeam2 = new QListWidget;
    // Fill the lists
    for (const auto &player : team1.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam1->addItem(item);
    }
    for (const auto &player : team2.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam2->addItem(item);
    }

    QHBoxLayout *listsLayout = new QHBoxLayout;
    listsLayout->addWidget(listTeam1);
    listsLayout->addWidget(listTeam2);
    mainLayout->addLayout(listsLayout);

    // --- Time and Player Fields ---
    QLineEdit *timeEdit = new QLineEdit;
    QLineEdit *playerEdit = new QLineEdit;
    playerEdit->setReadOnly(true); // Player field auto-filled

    // Pre-fill timeEdit with current timer time + 1
    unsigned minutes = 0;
    if (gametime)
    {
        QString elapsed;
        if (gametime->runningPeriod == true)
        {
            elapsed = gametime->firsthalf();
        }
        else if (gametime->runningPeriod == false)
        {
            elapsed = gametime->secondhalf();
        }
        minutes = elapsed.section(":", 0, 0).toInt() + 1;
    }
    timeEdit->setText(QString::number(minutes));

    QHBoxLayout *fieldsLayout = new QHBoxLayout;
    fieldsLayout->addWidget(new QLabel("Zeit (min):"));
    fieldsLayout->addWidget(timeEdit);
    fieldsLayout->addWidget(new QLabel("Spieler:"));
    fieldsLayout->addWidget(playerEdit);
    mainLayout->addLayout(fieldsLayout);

    // --- Update player field when a player is selected ---
    QString selectedTeam; // This part is to signal a own goal
    auto updatePlayerField = [&](QListWidgetItem *item, QString team)
    {
        playerEdit->setText(item->text());
        selectedTeam = team;
    };
    connect(listTeam1, &QListWidget::itemClicked, [=](QListWidgetItem *item)
            { updatePlayerField(item, "team1"); });
    connect(listTeam2, &QListWidget::itemClicked, [=](QListWidgetItem *item)
            { updatePlayerField(item, "team2"); });

    // --- Save Button ---
    QPushButton *SaveButton = new QPushButton("Save");
    QPushButton *CancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(SaveButton);
    buttonLayout->addWidget(CancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(CancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(SaveButton, &QPushButton::clicked, [&]()
            {
        QString timeStr = timeEdit->text();
        QString playerStr = playerEdit->text();

        // Extract player number and name
        int number = playerStr.section(" - ", 0, 0).toInt();
        QString name = playerStr.section(" - ", 1, 1);

        /***********************************************/
        bool isOwnGoal = false;
        if (selectedTeam == "team2") {
            // Confirm own goal
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(&dialog, "Own Goal?", "Is this an own goal?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                isOwnGoal = true;
            }
        }
        /***********************************************/

        ScoreMemory->addGoal(number, name, timeStr, "Home", isOwnGoal);
        qDebug() << "Goal added:" << number << name << timeStr;
        dialog.accept(); });
    dialog.exec();
}

void controll_window::AddGoalTeam2()
{
    /**
     * @brief By pressing the GOAL button, it pos up a new dialog window where a goal for the
     * team1 can be added. It automatically enters the time when the goal button was pressed.
     * But the time is adjustable. By chosing the player from the opponents lists,
     * it asks if it was a own goal. If it is a own goal, it will also be marked as one.
     * The time and scorer are returned to the score_memory class.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    qDebug() << "Goal for Home Team!";
    QDialog dialog(this);
    dialog.setWindowTitle("Tor Heimmannschaft");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    // --- Player Lists ---
    QListWidget *listTeam1 = new QListWidget;
    QListWidget *listTeam2 = new QListWidget;
    // Fill the lists
    for (const auto &player : team1.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam1->addItem(item);
    }
    for (const auto &player : team2.getPlayers())
    {
        QString item = QString::number(player->getNumber()) + " - " + player->getName();
        listTeam2->addItem(item);
    }

    QHBoxLayout *listsLayout = new QHBoxLayout;
    listsLayout->addWidget(listTeam1);
    listsLayout->addWidget(listTeam2);
    mainLayout->addLayout(listsLayout);

    // --- Time and Player Fields ---
    QLineEdit *timeEdit = new QLineEdit;
    QLineEdit *playerEdit = new QLineEdit;
    playerEdit->setReadOnly(true); // Player field auto-filled

    // Pre-fill timeEdit with current timer time + 1
    unsigned minutes = 0;
    if (gametime)
    {
        QString elapsed;
        if (gametime->runningPeriod == true)
        {
            elapsed = gametime->firsthalf();
        }
        else if (gametime->runningPeriod == false)
        {
            elapsed = gametime->secondhalf();
        }
        minutes = elapsed.section(":", 0, 0).toInt() + 1;
    }
    timeEdit->setText(QString::number(minutes));

    QHBoxLayout *fieldsLayout = new QHBoxLayout;
    fieldsLayout->addWidget(new QLabel("Zeit (min):"));
    fieldsLayout->addWidget(timeEdit);
    fieldsLayout->addWidget(new QLabel("Spieler:"));
    fieldsLayout->addWidget(playerEdit);
    mainLayout->addLayout(fieldsLayout);

    // --- Update player field when a player is selected ---
    QString selectedTeam; // This part is to signal a own goal
    auto updatePlayerField = [&](QListWidgetItem *item, QString team)
    {
        playerEdit->setText(item->text());
        selectedTeam = team;
    };
    connect(listTeam1, &QListWidget::itemClicked, [=](QListWidgetItem *item)
            { updatePlayerField(item, "team1"); });
    connect(listTeam2, &QListWidget::itemClicked, [=](QListWidgetItem *item)
            { updatePlayerField(item, "team2"); });

    // --- Save Button ---
    QPushButton *SaveButton = new QPushButton("Save");
    QPushButton *CancelButton = new QPushButton("Cancel");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(SaveButton);
    buttonLayout->addWidget(CancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(CancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(SaveButton, &QPushButton::clicked, [&]()
            {
        QString timeStr = timeEdit->text();
        QString playerStr = playerEdit->text();

        // Extract player number and name
        int number = playerStr.section(" - ", 0, 0).toInt();
        QString name = playerStr.section(" - ", 1, 1);

        /***********************************************/
        bool isOwnGoal = false;
        if (selectedTeam == "team1") {
            // Confirm own goal
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(&dialog, "Own Goal?", "Is this an own goal?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                isOwnGoal = true;
            }
        }
        /***********************************************/

        ScoreMemory->addGoal(number, name, timeStr, "Away", isOwnGoal);
        qDebug() << "Goal added:" << number << name << timeStr;
        dialog.accept(); });
    dialog.exec();
}

void controll_window::StartTime()
{
    /**
     * @brief Sends the signal to the timer class that the timer should start.
     * Also the start timer button will be set disabled, while the state of the timer is running.
     */
    if (!gametime->isRunning())
    {
        qDebug() << "Time is running";
        gametime->start();
        btnStartTimer->setDisabled(true);
    }
}

void controll_window::UpdateTimeDisplay(const QString &elapsedTime)
{
    /**
     * @brief This methode updates the time for displaying it in the
     * controll window for the operator.
     */
    qDebug() << "Current Time: " << elapsedTime;
    time->setText(elapsedTime);
}

void controll_window::UpdateScoreDisplay()
{
    /**
     * @brief This methode updates the score for displaying it in the
     * controll window for the operator.
     */
    unsigned homeScore = ScoreMemory->getHomeScore();
    unsigned awayScore = ScoreMemory->getAwayScore();
    score->setText(QString::number(homeScore) + " : " + QString::number(awayScore));
}

void controll_window::Log()
{
    /**
     * @brief The log button is there to reset the score, restart the timer and to
     * retake the last added goal
     *
     * @note this part of the code was coded whit help from chatgpt.
     */
    qDebug() << "Log button clicked";
    QDialog dialog(this);
    dialog.setWindowTitle("Log window");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QHBoxLayout *LogButtonLayout = new QHBoxLayout;
    QPushButton *RestartButton = new QPushButton("Restart");
    QPushButton *ResetButton = new QPushButton("Reset");
    QPushButton *UndoGoalButton = new QPushButton("Tor zurücknehmen");
    LogButtonLayout->addWidget(RestartButton);
    LogButtonLayout->addWidget(ResetButton);
    LogButtonLayout->addWidget(UndoGoalButton);

    QPushButton *CancelButton = new QPushButton("Cancel");
    CancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addLayout(LogButtonLayout);
    layout->addWidget(CancelButton);

    connect(RestartButton, &QPushButton::clicked, [&]()
            {
        qDebug() << "Restart clicked";
        dialog.accept();
        gametime->restart(); });

    connect(ResetButton, &QPushButton::clicked, [&]()
            {
        qDebug() << "Reset clicked";
        dialog.accept();
        gametime->stop();
        btnStartTimer->setDisabled(false);
        ScoreMemory->resetGame(); });

    connect(UndoGoalButton, &QPushButton::clicked, [&]()
            {
        qDebug() << "Tor zurücknehmen clicked";
        dialog.accept();
        ScoreMemory->removeLastGoal(); });

    connect(CancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void controll_window::setScoreMemory(score_memory *mem)
{
    /**
     * @brief updates the score to the score_memory class
     *
     * @note this part of the code was generated by chatgpt!
     */
    ScoreMemory = mem;
    connect(ScoreMemory, &score_memory::goalsUpdated, this, &controll_window::UpdateScoreDisplay);
}

void controll_window::setTimer(timer *t)
{
    /**
     * @brief Overgives the informations from timer class to controll_window class.
     *
     * @note this part of the code was generated by chatgpt!
     */
    gametime = t;
    connect(gametime, &timer::timeUpdated, this, &controll_window::UpdateTimeDisplay);
    connect(gametime, &timer::timeout, this, &controll_window::handleTimerTimeout);
}

/**********************************/
void controll_window::loadEmblemTeam1()
{
    /**
     * @brief Allows to upload a emblem of the team.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    emblemTeam1 = QFileDialog::getOpenFileName(this, "Vereinslogo einfügen", "", "Images((*.png *.jpg *.bmp)");
    if (!emblemTeam1.isEmpty())
    {
        emit emblemChanged("Home", emblemTeam1);
    }
}

void controll_window::loadEmblemTeam2()
{
    /**
     * @brief Allows to upload a emblem of the team.
     *
     * @note this part of the code was coded with help from chatgpt.
     */
    emblemTeam2 = QFileDialog::getOpenFileName(this, "Vereinslogo einfügen", "", "Images((*.png *.jpg *.bmp)");
    if (!emblemTeam2.isEmpty())
    {
        emit emblemChanged("Away", emblemTeam2);
    }
}

// import teams
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
void controll_window::ImportTeam1()
{
    /**
     * @brief Allows to upload a playerlist of the team in form of a csv-file.
     *
     * @note this part of the code was coded with help from chatgpt.
     * @note That the number and name of the player needs to be seperated by ";". -
     * (Excel Microsoft 365 MSO Version 2505)
     */
    QString fileName = QFileDialog::getOpenFileName(this, "Import Player List (Home Team)", "", "CSV Files (*.csv *.CSV);; All Files(*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "File Error", "Could not open file for reading.");
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(";");
        if (fields.size() >= 2)
        {
            bool ok;
            int number = fields[0].trimmed().toInt(&ok);
            QString name = fields[1].trimmed();
            if (ok && !name.isEmpty())
            {
                team1.addPlayer(number, name);
            }
        }
    }
    updateTeamList1();
}

void controll_window::ImportTeam2()
{
    /**
     * @brief Allows to upload a playerlist of the team in form of a csv-file.
     *
     * @note this part of the code was coded with help from chatgpt.
     * @note That the number and name of the player needs to be seperated by ";". -
     * (Excel Microsoft 365 MSO Version 2505)
     */
    QString fileName = QFileDialog::getOpenFileName(this, "Import Player List (Away Team)", "", "CSV Files (*.csv *.CSV);; All Files(*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "File Error", "Could not open file for reading.");
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(";");
        if (fields.size() >= 2)
        {
            bool ok;
            int number = fields[0].trimmed().toInt(&ok);
            QString name = fields[1].trimmed();
            if (ok && !name.isEmpty())
            {
                team2.addPlayer(number, name);
            }
        }
    }
    updateTeamList2();
}

void controll_window::applyStateSelection()
{
    /**
     * @date 23.11.2025
     * @brief Statemachine for the application to manage the scoreboard,
     *  what it has to display on the screen.
     */
    MatchState newState = m_currentState;

    if (radioPreGame->isChecked()) {
        newState = MatchState::PreGame;
    } else if (radioFirstHalf->isChecked()) {
        newState = MatchState::FirstHalf;
    } else if (radioHalfTime->isChecked()) {
        newState = MatchState::HalfTime;
    } else if (radioSecondHalf->isChecked()) {
        newState = MatchState::SecondHalf;
    } else if (radioPostGame->isChecked()) {
        newState = MatchState::PostGame;
    }

    // Nothing changed → do nothing
    if (newState == m_currentState) {
        return;
    }

    m_currentState = newState;

    // Notify others (e.g. Score_board) later
    emit matchStateChanged(static_cast<int>(m_currentState));
}


// Starting a second have of a game.
void controll_window::handleTimerTimeout()
{
    /**
     * @brief After 45min it popsup a new window, where it asks to continue the timer with the
     * second half.
     *
     */
    btnStartTimer->setDisabled(false);
    if (gametime->runningPeriod == true)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Second Half", "Do you want to start the second half?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            StartTime(); // Start the timer for the second half
        }
        else
        {
            // Do nothing; timer remains stopped
            qDebug() << "Second half not started.";
        }
    }
}
/**********************************/

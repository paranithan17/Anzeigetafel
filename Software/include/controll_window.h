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
#ifndef CONTROLL_WINDOW_H
#define CONTROLL_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QGroupBox>
#include <QMouseEvent>

#include "home_team.h"
#include "away_team.h"
#include "timer.h"
#include "score_memory.h"

class controll_window : public QWidget
{
    Q_OBJECT

public:
    // Simple state machine for the match
    enum class MatchState
    {
        PreGame = 0,
        FirstHalf,
        HalfTime,
        SecondHalf,
        PostGame
    };

private:
    // Current state (default: PreGame)
    MatchState m_currentState = MatchState::PreGame;

    // Elements for connect with other classes
    home_team team1;
    away_team team2;

    timer *gametime;

    score_memory *ScoreMemory;

    // GUI elements for playermanagement
    QPushButton *btnAddPlayerTeam1;
    QPushButton *btnAddPlayerTeam2;

    // GUI  elements for goalmanagement
    QPushButton *btnGoalTeam1;
    QPushButton *btnGoalTeam2;

    // GUI elements for timer-start and log buttons
    QPushButton *btnStartTimer;
    QPushButton *btnLog;

    // GUI elements for match state selection (state machine)
    QRadioButton *radioPreGame;
    QRadioButton *radioFirstHalf;
    QRadioButton *radioHalfTime;
    QRadioButton *radioSecondHalf;
    QRadioButton *radioPostGame;
    QPushButton *btnApplyState;

    QListWidget *listTeam1;
    QListWidget *listTeam2;

    // Time and score for the operator
    QLabel *score;
    QLabel *time;

    /**********************************/
    /**
     * Extra features:
     * 1) Adding emblems of both teams
     */

    QPushButton *btnAddEmblemTeam1;
    QPushButton *btnAddEmblemTeam2;
    QString emblemTeam1;
    QString emblemTeam2;

    /**
     * 2) Import player list with csv-file
     */
    QPushButton *btnImportTeam1;
    QPushButton *btnImportTeam2;

    /**********************************/

    // MEthods
    void updateTeamList1();
    void updateTeamList2();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void AddPlayerTeam1();
    void AddPlayerTeam2();
    void RemovePlayerTeam1(QListWidgetItem *item);
    void RemovePlayerTeam2(QListWidgetItem *item);

    void AddGoalTeam1();
    void AddGoalTeam2();

    void StartTime();
    void UpdateTimeDisplay(const QString &elapsedTime);
    void UpdateScoreDisplay();
    void Log();

    /**********************************/
    void loadEmblemTeam1();
    void loadEmblemTeam2();

    void ImportTeam1();
    void ImportTeam2();

    // Confirm the selected match state
    void applyStateSelection();

    /**
     * 3) start second leg
     */
    void handleTimerTimeout();
    /**********************************/

public:
    controll_window(QWidget *parent = nullptr);
    void setScoreMemory(score_memory *mem);
    void setTimer(timer *t);
    void setScoreboard(QWidget *board);
    void toggleScoreboard();

private:
    QWidget *scoreboard = nullptr;

signals:
    void emblemChanged(const QString &team, const QString &filepath);

    // Emits whenever the operator confirms a new match state
    void matchStateChanged(int state); // static_cast<int>(MatchState)
};

#endif // CONTROLL_WINDOW_H

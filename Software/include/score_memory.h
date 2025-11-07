/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  score_memory.h
 * @class score_memory
 * @caption Main operator which handles the logic when actions in control_window are triggered.
 * The score_memory stores data (scores, goals, event time, player)
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 27.05.2025
 *
 * @note This code has been created with help of chatgpt
 */
#ifndef SCORE_MEMORY_H
#define SCORE_MEMORY_H

#include <QObject>
#include <QString>
#include <QList>

struct Goal
{
    int playerNumber;
    QString player;
    QString timeStamp;
    QString team;
    bool ownGoal = false;
};

class score_memory : public QObject
{
    Q_OBJECT

private:
    QList<Goal> goals;

public:
    score_memory(QObject *parent = nullptr);

    QList<Goal> getGoals() const;
    int getHomeScore() const;
    int getAwayScore() const;

    void addGoal(int number, QString player, const QString &timeStamp, const QString &team, bool ownGoal);
    void removeLastGoal();
    void resetGame();

signals:
    void goalsUpdated();
};

#endif // SCORE_MEMORY_H

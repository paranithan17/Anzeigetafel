/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  score_memory.cpp
 * @class score_memory
 * @caption The score_memory stores data (scores, goals, event time, player),
 * which provides data to controll_window and score_board
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 27.05.2025
 *
 *
 * @note This code has been created with help of chatgpt
 */
#include "score_memory.h"

score_memory::score_memory(QObject *parent)
    : QObject(parent)
{
}

QList<Goal> score_memory::getGoals() const
{
    return goals;
}

int score_memory::getHomeScore() const
{
    /**
     * @brief Memorizes the score of the Team 1
     */
    int count = 0;
    for (const Goal &g : goals)
    {
        if ((g.team == "Home" && !g.ownGoal) || (g.team == "Away" && g.ownGoal))
            count++;
    }
    return count;
}

int score_memory::getAwayScore() const
{
    /**
     * @brief Memorizes the score of the Team 2
     */
    int count = 0;
    for (const Goal &g : goals)
    {
        if ((g.team == "Away" && !g.ownGoal) || (g.team == "Home" && g.ownGoal))
            count++;
    }
    return count;
}

void score_memory::addGoal(int number, QString player, const QString &timeStamp, const QString &team, bool ownGoal)
{
    /**
     * @brief adds the Goal events of the full game.
     */
    goals.append({number, player, timeStamp, team, ownGoal});
    emit goalsUpdated();
}

void score_memory::removeLastGoal()
{
    /**
     * @brief Removes the previous goal in the memory.
     */
    if (!goals.isEmpty())
    {
        goals.removeLast();
        emit goalsUpdated();
    }
}

void score_memory::resetGame()
{
    /**
     * @brief Resets the goals totally
     */
    goals.clear();
    emit goalsUpdated();
}

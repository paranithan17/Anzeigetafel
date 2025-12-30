/**
 * @file away_team.cpp
 * @brief Implementation of away team roster management.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-01
 */

#include "away_team.h"
#include "player.h"
#include "algorithm"
#include "QDebug"

away_team::away_team() {}

void away_team::addPlayer(unsigned number, const QString &name)
{
    auto newPlayer = std::make_shared<player>(number, name);
    players.push_back(newPlayer);
    qDebug() << "(away) " << number << " " << name << "\n";
}

void away_team::removePlayer(unsigned number)
{
    QString removedName;
    bool found = false;
    auto it = std::remove_if(players.begin(), players.end(),
                             [&](const std::shared_ptr<player> &p)
                             {
                                 if (p->getNumber() == number)
                                 {
                                     removedName = p->getName();
                                     found = true;
                                     return true;
                                 }
                                 return false;
                             });
    if (found)
    {
        players.erase(it, players.end());
        qDebug() << "Removed player " << removedName << " " << number << "\n";
    }
    else
    {
        qDebug() << "Player with number: " << number << " does not exist.";
    }
}

const std::vector<std::shared_ptr<player>> &away_team::getPlayers() const
{
    return players;
}

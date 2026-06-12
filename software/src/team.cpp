/**
 * @file team.cpp
 * @brief Implementation of team roster management.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */

#include "team.h"

#include <algorithm>

#include <QDebug>

team::team()
{
}

void team::addPlayer(unsigned number, const QString &name)
{
    auto newPlayer = std::make_shared<player>(number, name);
    players.push_back(newPlayer);
    qDebug() << "Added player" << number << name;
}

void team::removePlayer(unsigned number)
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
        qDebug() << "Removed player" << removedName << number;
    }
    else
    {
        qDebug() << "Player with number:" << number << "does not exist.";
    }
}

const std::vector<std::shared_ptr<player>> &team::getPlayers() const
{
    return players;
}
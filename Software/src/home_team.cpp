/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  home_team.cpp
 * @class home_team
 * @brief Mananges the playerlist of the hometeam
 *
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 27.05.2025
 * @version V2.0, 01.06.2025 - std::string changed to QString
 *
 * @note This code has been created with help of chatgpt
 *
 * @todo At the moment, there is no check if a number is already taken
 *
 */
#include "home_team.h"
#include "player.h"
#include "algorithm"
#include "QDebug"

home_team::home_team()
{
}

void home_team::addPlayer(unsigned number, const QString &name)
{
    auto newPlayer = std::make_shared<player>(number, name);
    players.push_back(newPlayer);
    qDebug() << "(home) " << number << " " << name << "\n";
}

void home_team::removePlayer(unsigned number)
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
                                     return true; // marks the removal
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

const std::vector<std::shared_ptr<player>> &home_team::getPlayers() const
{
    return players;
}

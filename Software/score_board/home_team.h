/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * @file  home_team.h
 * @class home_team
 * @caption Mananges the playerlist of the hometeam
 *
 *
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 27.05.2025
 * @version V2.0, 01.06.2025 - std::string changed to QString
 *
 *
 * @note This code has been created with help of chatgpt
 */
#ifndef HOME_TEAM_H
#define HOME_TEAM_H

#include <vector>
#include <memory>
#include <Person.h>

class home_team
{
private:
    std::vector<std::shared_ptr<Person>> players ;
public:
    home_team();
    void addPlayer(unsigned number, const QString& name);
    void removePlayer(unsigned number);
    const std::vector<std::shared_ptr<Person>>& getPlayers() const;
};

#endif // HOME_TEAM_H

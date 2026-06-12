/**
 * @file team.h
 * @brief Team roster management.
 *
 * Represents a team that consists of player objects and provides basic roster
 * operations such as add, remove, and read access to all players.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */
#ifndef TEAM_H
#define TEAM_H

#include <memory>
#include <vector>

#include "player.h"

class team
{
public:
    /**
     * @brief Constructs empty team roster.
     */
    team();

    /**
     * @brief Adds player to team roster.
     *
     * @param number Player jersey number
     * @param name Player name
     */
    void addPlayer(unsigned number, const QString &name);

    /**
     * @brief Removes player from team roster by jersey number.
     *
     * @param number Jersey number of player to remove
     */
    void removePlayer(unsigned number);

    /**
     * @brief Gets all players in team roster.
     *
     * @return Const reference to vector of player shared pointers
     */
    const std::vector<std::shared_ptr<player>> &getPlayers() const;

private:
    std::vector<std::shared_ptr<player>> players;
};

#endif // TEAM_H
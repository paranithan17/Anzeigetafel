/**
 * @file home_team.h
 * @brief Home team roster management.
 *
 * Manages the home team's player list with add/remove operations.
 * Players are stored as shared pointers for efficient memory management.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-01
 */
#ifndef HOME_TEAM_H
#define HOME_TEAM_H

#include <vector>
#include <memory>
#include "player.h"

class home_team
{
public:
    /**
     * @brief Constructs empty home team roster.
     */
    home_team();

    /**
     * @brief Adds player to home team roster.
     *
     * @param number Player jersey number
     * @param name Player name
     */
    void addPlayer(unsigned number, const QString &name);

    /**
     * @brief Removes player from home team roster by jersey number.
     *
     * @param number Jersey number of player to remove
     */
    void removePlayer(unsigned number);

    /**
     * @brief Gets all players in home team roster.
     *
     * @return Const reference to vector of player shared pointers
     */
    const std::vector<std::shared_ptr<player>> &getPlayers() const;

private:
    std::vector<std::shared_ptr<player>> players;
};

#endif // HOME_TEAM_H

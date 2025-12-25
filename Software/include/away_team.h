/**
 * @file away_team.h
 * @brief Away team roster management.
 *
 * Manages the away team's player list with add/remove operations.
 * Players are stored as shared pointers for efficient memory management.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-01
 */
#ifndef AWAY_TEAM_H
#define AWAY_TEAM_H

#include <vector>
#include <memory>
#include "player.h"

class away_team
{
public:
    /**
     * @brief Constructs empty away team roster.
     */
    away_team();

    /**
     * @brief Adds player to away team roster.
     *
     * @param number Player jersey number
     * @param name Player name
     */
    void addPlayer(unsigned number, const QString &name);

    /**
     * @brief Removes player from away team roster by jersey number.
     *
     * @param number Jersey number of player to remove
     */
    void removePlayer(unsigned number);

    /**
     * @brief Gets all players in away team roster.
     *
     * @return Const reference to vector of player shared pointers
     */
    const std::vector<std::shared_ptr<player>> &getPlayers() const;

private:
    std::vector<std::shared_ptr<player>> players;
};

#endif // AWAY_TEAM_H

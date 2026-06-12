/**
 * @file player.h
 * @brief Player data model for roster management.
 *
 * Represents a single player with identification number and name.
 * Used by team rosters (team) for tracking player information.
 * Players are managed through shared pointers for efficient memory management and
 * automatic cleanup.
 *
 * Features:
 * - Player number and name storage
 * - Getter and setter methods for player attributes
 * - Lightweight data model for roster management
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 3.0, 12. June 2026
 */

#ifndef PLAYER_H
#define PLAYER_H
#include <QString>

class player
{
public:
    /**
     * @brief Constructs a player with number and name.
     *
     * @param number Jersey number
     * @param name Player name
     */
    player(unsigned number, QString name);

    /**
     * @brief Gets player name.
     *
     * @return Player name
     */
    QString getName();

    /**
     * @brief Gets player jersey number.
     *
     * @return Jersey number
     */
    unsigned getNumber();

    /**
     * @brief Sets player jersey number.
     *
     * @param number New jersey number
     */
    void setNumber(unsigned number);

    /**
     * @brief Sets player name.
     *
     * @param name New player name
     */
    void setName(QString name);

private:
    unsigned playerNumber;
    QString playerName;
};

#endif // PLAYER_H

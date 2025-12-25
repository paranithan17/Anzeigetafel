/**
 * @file player.h
 * @brief Player data model for roster management.
 *
 * Represents a player with number and name.
 * Used by team rosters for tracking player information.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-01
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

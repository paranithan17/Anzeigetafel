/**
 * @file player.cpp
 * @brief Implementation of player data model.
 *
 * @author Paranithan Paramalingam (BFH-Ti)
 * @version 2.0, 2025-06-01
 */

#include "player.h"

player::player(unsigned number, QString name) : playerNumber(number),
                                                playerName(name)
{
}

QString player::getName()
{ 
    return playerName;
}

unsigned player::getNumber()
{
    return playerNumber;
}

void player::setName(QString name)
{
    playerName = name;
}

void player::setNumber(unsigned number)
{
    playerNumber = number;
}

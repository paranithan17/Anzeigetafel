/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * Class player,
 * Initializes the player with their Name and number.
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 *
 *
 */

#include <player.h>

// constructor
player::player(unsigned number, QString name) : playerNumber(number),
                                                playerName(name)
{
}

// methods
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

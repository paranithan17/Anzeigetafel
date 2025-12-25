/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * Class player,
 * Initializes the player with their Name and number.
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 * @version V2.0, 01.06.2025 - changed std::string to QString
 *
 */

#ifndef PLAYER_H
#define PLAYER_H
#include <QString>
class player
{
private:
    unsigned playerNumber;
    QString playerName;

public:
    player(unsigned number, QString name);

    QString getName();
    unsigned getNumber();

    void setNumber(unsigned number);
    void setName(QString name);
};

#endif // PLAYER_H

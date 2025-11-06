/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * Class Person,
 * Initialzes the player with his Name and number.
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 *
 *
 */

#include <Person.h>


// constructor
Person::Person(unsigned number, QString name):
    P_number(number),
    P_name(name)
{
}


// methods
QString Person::getName()
{
    return  P_name;
}

unsigned Person::getNumber()
{
    return P_number;
}

void Person::setName(QString name)
{
    P_name = name;
}

void Person::setNumber(unsigned number)
{
    P_number = number;
}

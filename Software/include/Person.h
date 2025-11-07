/**
 * BTE5058a Einstieg in  OOP, Mini-project Scoreboard
 * Class Person,
 * Initialzes the player with his Name and number.
 * @author Paranithan Paramalingam. BFH-Ti
 * @version V1.0, 26.05.2025
 * @version V2.0, 01.06.2025 - changed std::string to QString
 *
 */


#ifndef PERSON_H
#define PERSON_H

#include <QString>
class Person
{
private:
    unsigned P_number;
    QString P_name;

public:
    Person(unsigned number, QString name);


    QString getName();
    unsigned getNumber();

    void setNumber(unsigned number);
    void setName(QString name);

};

#endif // PERSON_H

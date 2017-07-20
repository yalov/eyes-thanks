#ifndef XML_H
#define XML_H

#include "QUtfString.h"

class CharacterSets
{
    struct Set {
        int index = 0;
        QString name;
        QString comment;
        QString title;
        QString characters;
    };

    QList<Set> * sets;

public:
    CharacterSets(const QString& filepath);
    void write(const QString &filepath);
    void read(const QString& filepath);
    int size();
    QUtfString get_title(int index);
    QUtfString get_characters(int index);


};

#endif // XML_H

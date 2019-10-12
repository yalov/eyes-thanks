#ifndef XML_H
#define XML_H

#include <QList>
#include "QUtfString.h"

class CharacterSets
{
    struct Set {
        int index = 0;
        QString name{};
        QString comment{};
        QString title{};
        QString characters{};
        QList<QString> compositecharacters{};
    };

    QList<Set> * sets;

public:
    CharacterSets(const QString& filepath);
    ~CharacterSets();
    void write(const QString &filepath);
    void read(const QString& filepath);
    int size();
    QUtfString get_title(int index);
    QList<QUtfString> get_characters(int index);


};

#endif // XML_H

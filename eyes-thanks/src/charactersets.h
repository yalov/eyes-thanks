#ifndef XML_H
#define XML_H

#include <QList>
#include "QUtfString.h"

class CharacterSets
{

    public:
        CharacterSets(const QString& filepath);
        ~CharacterSets();
        void write(const QString &filepath);
        void read(const QString& filepath);

        int size();
        QUtfString get_title(int index);
        QUtfString get_characters(int index);

    private:
        struct Set;
        QList<Set> * sets{};
        int randomSet {-1};
};

#endif // XML_H

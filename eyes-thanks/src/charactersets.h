#ifndef XML_H
#define XML_H

#include <QList>

class CharacterSets
{
    public:
        CharacterSets(const QString& filepath);
        ~CharacterSets();
        void write(const QString &filepath);
        void read(const QString& filepath);

        int size();
        QString get_title(int index);
        QString get_characters(int index);
        int ChooseRandomCurrentSet();
        QList<QString> currentCharacters;
        QList<QString> currentTitle;

    private:
        struct Set;
        QList<Set> * sets{};

        int randomSetIndex {-1};
};

#endif // XML_H

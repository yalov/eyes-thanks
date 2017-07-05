#ifndef TRANSLITERATIONISO9A_H
#define TRANSLITERATIONISO9A_H

#include <QString>

inline static bool check_rus_compatibility(QString input)
{
    bool compatibility = true;
    for (auto letter : input) {
        if (letter.isLetter()) {
            QChar u = letter;
            if ((u.toLower() < L'а' || u.toLower() > L'я')
                    && (u.toLower() != L'є')
                    && (u.toLower() != L'ё')
                    && (u.toLower() != L'і')
                    && (u.toLower() != L'ї')
                    && (u.toLower() != L'ў')
                    && (u.toLower() != L'ґ')) {
                compatibility = false;
                break;
            }
        }
    }
    return compatibility;
}

inline static QString transliteraction(QString input)
{
    if (check_rus_compatibility(input)) {
        QString output;
        for (auto letter : input) {
            QChar tr;

            if      (letter.toLower() == L'а') tr = QChar(L'a');
            else if (letter.toLower() == L'б') tr = QChar(L'b');
            else if (letter.toLower() == L'в') tr = QChar(L'v');
            else if (letter.toLower() == L'г') tr = QChar(L'g');
            else if (letter.toLower() == L'д') tr = QChar(L'd');
            else if (letter.toLower() == L'е') tr = QChar(L'e');
            else if (letter.toLower() == L'ж') tr = QChar(L'ž');
            else if (letter.toLower() == L'з') tr = QChar(L'z');
            else if (letter.toLower() == L'и') tr = QChar(L'i');
            else if (letter.toLower() == L'й') tr = QChar(L'j');
            else if (letter.toLower() == L'к') tr = QChar(L'k');
            else if (letter.toLower() == L'л') tr = QChar(L'l');
            else if (letter.toLower() == L'м') tr = QChar(L'm');
            else if (letter.toLower() == L'н') tr = QChar(L'n');
            else if (letter.toLower() == L'о') tr = QChar(L'o');
            else if (letter.toLower() == L'п') tr = QChar(L'p');
            else if (letter.toLower() == L'р') tr = QChar(L'r');
            else if (letter.toLower() == L'с') tr = QChar(L's');
            else if (letter.toLower() == L'т') tr = QChar(L't');
            else if (letter.toLower() == L'у') tr = QChar(L'u');
            else if (letter.toLower() == L'ф') tr = QChar(L'f');
            else if (letter.toLower() == L'х') tr = QChar(L'h');
            else if (letter.toLower() == L'ц') tr = QChar(L'c');
            else if (letter.toLower() == L'ч') tr = QChar(L'č');
            else if (letter.toLower() == L'ш') tr = QChar(L'š');
            else if (letter.toLower() == L'щ') tr = QChar(L'ŝ');
            else if (letter.toLower() == L'ъ') tr = QChar(L'ʺ');
            else if (letter.toLower() == L'ы') tr = QChar(L'y');
            else if (letter.toLower() == L'ь') tr = QChar(L'ʹ');
            else if (letter.toLower() == L'э') tr = QChar(L'è');
            else if (letter.toLower() == L'ю') tr = QChar(L'û');
            else if (letter.toLower() == L'я') tr = QChar(L'â');
            else if (letter.toLower() == L'є') tr = QChar(L'ê');
            else if (letter.toLower() == L'ё') tr = QChar(L'ë');
            else if (letter.toLower() == L'і') tr = QChar(L'ì');
            else if (letter.toLower() == L'ї') tr = QChar(L'ï');
            else if (letter.toLower() == L'ў') tr = QChar(L'ŭ');
            else if (letter == L'Ґ') output += 'G' + QChar(0x0300);
            else if (letter == L'ґ') output += 'g' + QChar(0x0300);
            else if (letter == QChar(0x0027)
                  || letter == QChar(0x2019)
                  || letter == QChar(0x02BC)) output += QChar(0x02BC);
            else tr = letter;

            if (letter.isUpper())
                tr = tr.toUpper();

            output += tr;

        }
        return QString(output);
    }
    else
        return input;

}

#endif // TRANSLITERATIONISO9A_H


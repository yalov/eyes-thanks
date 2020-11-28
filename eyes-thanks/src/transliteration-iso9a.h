#ifndef TRANSLITERATIONISO9A_H
#define TRANSLITERATIONISO9A_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVector>

inline bool check_rus_compatibility(const QString& input)
{
    bool compatibility = true;
    for (auto letter : input) {
        if (letter.isLetter()) {
            QChar u = letter;
            if ((u.toLower() < QChar(0x0430) || u.toLower() > QChar(0x044F))  // <а >я
                    && (u.toLower() != QChar(0x0454)) // є
                    && (u.toLower() != QChar(0x0451)) // ё
                    && (u.toLower() != QChar(0x0456)) // і
                    && (u.toLower() != 'i')           // i - lat
                    && (u.toLower() != QChar(0x0457)) // ї
                    && (u.toLower() != QChar(0x045E)) // ў
                    && (u.toLower() != QChar(0x0491)) // ґ
                    && (u != QChar(0x02BC))) {
                compatibility = false;
                break;
            }
        }
    }
    return compatibility;
}

inline QString transliteraction(const QString& input)
{
    if (!check_rus_compatibility(input)) return input;

    QFile file(":res/iso9a.txt");
    QVector<QStringList> table;

    if(file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        while(!in.atEnd())
            table.append(in.readLine().split(" "));
        file.close();
    }

    QString output;
    for (auto letter : input) {
        QString ltr_out;

        for (auto rule: table)
            if (letter.toLower() == rule[0]) ltr_out = rule[1];
        if (letter.isUpper()) ltr_out = ltr_out.toUpper();
        if (ltr_out.isEmpty())     ltr_out = letter;

        output += ltr_out;
    }
    return output;

}

#endif // TRANSLITERATIONISO9A_H


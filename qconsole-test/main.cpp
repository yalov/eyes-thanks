#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QString>
#include <QXmlStreamWriter>
#include <QFile>
#include <QTextStream>
#include <QRect>

#include <QtWidgets/QApplication>

#include "transliteration-iso9a.h"
#include "global.h"


//inline QString transliteraction(QString input)
//{
//    if (true) {
//        QString output;
//        for (auto letter : input) {
//            QChar tr;

//            if      (letter.toLower() == "а") tr = QChar('a');
//            else if (letter.toLower() == L'б') tr = QChar(L'b');
//            else if (letter.toLower() == L'в') tr = QChar(L'v');
//            else if (letter.toLower() == L'г') tr = QChar(L'g');
//            else if (letter.toLower() == L'д') tr = QChar(L'd');
//            else if (letter.toLower() == L'е') tr = QChar(L'e');
//            else if (letter.toLower() == L'ж') tr = QChar(L'ž');
//            else if (letter.toLower() == L'з') tr = QChar(L'z');
//            else if (letter.toLower() == L'и') tr = QChar(L'i');
//            else if (letter.toLower() == L'й') tr = QChar(L'j');
//            else if (letter.toLower() == L'к') tr = QChar(L'k');
//            else if (letter.toLower() == L'л') tr = QChar(L'l');
//            else if (letter.toLower() == L'м') tr = QChar(L'm');
//            else if (letter.toLower() == L'н') tr = QChar(L'n');
//            else if (letter.toLower() == L'о') tr = QChar(L'o');
//            else if (letter.toLower() == L'п') tr = QChar(L'p');
//            else if (letter.toLower() == L'р') tr = QChar(L'r');
//            else if (letter.toLower() == L'с') tr = QChar(L's');
//            else if (letter.toLower() == L'т') tr = QChar(L't');
//            else if (letter.toLower() == L'у') tr = QChar(L'u');
//            else if (letter.toLower() == L'ф') tr = QChar(L'f');
//            else if (letter.toLower() == L'х') tr = QChar(L'h');
//            else if (letter.toLower() == L'ц') tr = QChar(L'c');
//            else if (letter.toLower() == L'ч') tr = QChar(L'č');
//            else if (letter.toLower() == L'ш') tr = QChar(L'š');
//            else if (letter.toLower() == L'щ') tr = QChar(L'ŝ');
//            else if (letter.toLower() == L'ъ') tr = QChar(L'ʺ');
//            else if (letter.toLower() == L'ы') tr = QChar(L'y');
//            else if (letter.toLower() == L'ь') tr = QChar(L'ʹ');
//            else if (letter.toLower() == L'э') tr = QChar(L'è');
//            else if (letter.toLower() == L'ю') tr = QChar(L'û');
//            else if (letter.toLower() == L'я') tr = QChar(L'â');
//            else if (letter.toLower() == L'є') tr = QChar(L'ê');
//            else if (letter.toLower() == L'ё') tr = QChar(L'ë');
//            else if (letter.toLower() == L'і') tr = QChar(L'ì');
//            else if (letter.toLower() == L'ї') tr = QChar(L'ï');
//            else if (letter.toLower() == L'ў') tr = QChar(L'ŭ');
//            else if (letter == L'Ґ') output += 'G' + QChar(0x0300);
//            else if (letter == L'ґ') output += 'g' + QChar(0x0300);
//            else if (letter == QChar(0x0027)
//                  || letter == QChar(0x2019)
//                  || letter == QChar(0x02BC)) output += QChar(0x02BC);
//            else tr = letter;

//            if (letter.isUpper())
//                tr = tr.toUpper();

//            output += tr;

//        }
//        return QString(output);
//    }
//    else
//        return input;

//}

//QString transliteraction2(QString input)
//{
//    QFile file("c:/Users/User/Repositories/eyes-thanks/repository/eyes-thanks/res/iso9a.txt");
//    QVector<QStringList> str;

//    if(file.open(QIODevice::ReadOnly)) {
//        QTextStream in(&file);
//        in.setCodec("UTF-8");
//        while(!in.atEnd()) {
//            QString line = in.readLine();
//            QStringList fields = line.split(" ");
//            str.append(fields);
//        }
//        file.close();
//    }

//    QString output;
//    for (auto letter : input) {
//        QString tr;

//        for (auto st: str)
//            if (letter.toLower() == st[0]) tr = st[1];
//        if (letter.isUpper()) tr = tr.toUpper();

//        if (tr.isEmpty())     tr = letter;

//        output += tr;
//    }
//    return output;
//}

void f1 () {
    QVector<QString> list = {
        QString(u8"Афанасий Ефре'мов")                 ,
        QString(u8"Вячеслав Коновалов")               ,
        QString(u8"Максим Назаров")                   ,
        QString(u8"Филипп Ґалубёў")                   ,
        QString(u8"Толстой Кирилл Эдуардович")        ,
        QString(u8"Караев Мстислав Евгениевич")       ,
        QString(u8"Шепкин Марк Касьянович")           ,
        QString(u8"Щеголяев Адриан Матвеевич")        ,
        QString(u8"Давыдов Тимур Святославович")      ,
        QString(u8"Разуваев Платон Елизарович")       ,
        QString(u8"Буркин Ипатий Венедиктович")       ,
        QString(u8"Грибков Яков Левович")             ,
        QString(u8"Остальцев Руслан Федосиевич")      ,
        QString(u8"Кортнев Иннокентий Викентиевич")   ,
        QString(u8"Хоботилов Сергей Олегович")        ,
        QString(u8"Горюшин Евгений Игнатиевич")       ,
        QString(u8"Александров Пимен Ильевич")        ,
        QString(u8"Кобзев Данила Елисеевич")          ,
        QString(u8"Мозговой Осип Пахомович")          ,
        QString(u8"Кошкин Измаил Платонович")         ,
        QString(u8"Гадолин Виссарион Кондратович")    ,
        QString(u8"Шостенко Ким Куприянович")         ,
        QString(u8"Другов Эммануил Мечиславович")     ,
        QString(u8"Бенедиктов Владимир Даниилович")   ,
        QString(u8"Коротаев Дмитрий Тарасович")       ,
        QString(u8"Онегин Никон Филимонович")         ,
        QString(u8"Верясов Рюрик Казимирович")        ,
        QString(u8"Явленский Вячеслав Никонович")     ,
        QString(u8"Сиясинов Прохор Тихонович")        ,
        QString(u8"Агапов Касьян Ипатиевич")          ,
        QString(u8"Житков Леондий Валериевич")        ,
        QString(u8"Яхин Григорий Давидович")          ,
        QString(u8"Пушкарёв Александр Мирославович")  ,
        QString(u8"Эссен Валентин Измаилович")        ,
        QString(u8"Щетинин Радион Трофимович")        ,
        QString(u8"Лясковец Данил Брониславович")     ,
        QString(u8"Земляков Аким Ермолаевич")         ,
        QString(u8"Милютин Даниил Сидорович")         ,
        QString(u8"Гершкович Антип Моисеевич")        ,
        QString(u8"Содовский Кузьма Михеевич")};

    for (auto name: list) {
        qDebug() << list.indexOf(name);
        LogToFile("names.txt", QString("%1; %2; %3")
                  .arg(name,40)
                  .arg(transliteraction(name),40)
                  .arg(transliteraction2(name),40)
                  );
}
}

//void f2 () {
//    QString str = u8"ЩZҐ";
//    QChar z = u'Z';
//    QChar g = u'Ґ';

//    if (g == "Ґ")  qDebug() <<"g ="<< g << "is \"Ґ\"";
//    if (g == u'Ґ') qDebug() <<"g ="<< g << "is \'Ґ\'";

//    if (z == "Z") qDebug()<<"z =" << z << "is \"Z\"";
//    if (z == 'Z') qDebug()<<"z =" << z << "is \'Z\'";

//    for (QChar c:str)
//    {
//        if (c == "Ґ") qDebug()<<"c =" << c << "is \"Ґ\"";
//        if (c == u'Ґ') qDebug()<<"c =" << c << "is \'Ґ\'";

//        if (c == "Z") qDebug()<<"c =" << c << "is \"Z\"";
//        if (c == 'Z') qDebug()<<"c =" << c << "is \'Z\'";
//    }
//}

void f3() {
    QString str = QString("'’") + QChar(0x02BC);
    for (auto c:str)
    {
        qDebug() << "islower" << c.isLower() <<"isletter" << c.isLetter();
    }
}

struct CharacterSet {
    int index = 0;
    QString name;
    QString comment;
    QString title;
    QString characters;
};

typedef QList<CharacterSet> CharacterSets;

inline void writeXml(CharacterSets characterSets, QString filepath)
{
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {

        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("sets");
        for (int i=0; i< characterSets.size(); i++) {
            stream.writeStartElement("set");
            stream.writeAttribute("index", QString::number(i));
            stream.writeAttribute("name", characterSets[i].name);
            stream.writeTextElement("comment", characterSets[i].comment);
            stream.writeTextElement("title", characterSets[i].title);
            stream.writeTextElement("characters", characterSets[i].characters);
            stream.writeEndElement();
        }
        stream.writeEndElement();
        stream.writeEndDocument();
        file.close();
    }
}

inline CharacterSets readXml(QString filepath)
{
    CharacterSets sets;
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(file.readAll());
        file.close();

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "set")
            {
                CharacterSet c;

                for(auto &attr: xml.attributes()) {
                    if (attr.name().toString() == QLatin1String("index")) {
                        c.index = attr.value().toInt();
                    }
                    if (attr.name().toString() == QLatin1String("name")) {
                        c.name = attr.value().toString();

                    }
                }

               xml.readNextStartElement();
               c.comment = xml.readElementText();
               xml.readNextStartElement();
               c.title = xml.readElementText();
               xml.readNextStartElement();
               c.characters = xml.readElementText();

               xml.skipCurrentElement();
               sets.push_back(c);
            }
        }
        if (xml.hasError()) {
            qDebug() << "do error handling";
        }

    }
    return sets;
}

inline QString str_from(QRect r)
{
    return QString("(%1,%2)[%3x%4]").arg(r.x(), 5).arg(r.y(), 5).arg(r.width(), 4).arg(r.height(), 4);
}

void f4()
{
    qDebug() << str_from(QApplication::desktop()->geometry());
}




int main(int argc, char *argv[])
{
    QApplication  a(argc, argv);

    f1();

}

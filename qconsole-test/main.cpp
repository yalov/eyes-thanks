#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QString>
#include <QXmlStreamWriter>

#include "transliteration-iso9a.h"
#include "global.h"


void f1 () {
    QVector<QString> list = {
        QString(u8"Афанасий Ефремов")                 ,
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
        LogToFile("names.txt", QString("%1; %2")
                  .arg(name,40)
                  .arg(transliteraction(name),40));
}
}

void f2 () {
    QString str = u8"ЩZҐ";
    QChar z = u'Z';
    QChar g = u'Ґ';

    if (g == "Ґ")  qDebug() <<"g ="<< g << "is \"Ґ\"";
    if (g == u'Ґ') qDebug() <<"g ="<< g << "is \'Ґ\'";

    if (z == "Z") qDebug()<<"z =" << z << "is \"Z\"";
    if (z == 'Z') qDebug()<<"z =" << z << "is \'Z\'";

    for (QChar c:str)
    {
        if (c == "Ґ") qDebug()<<"c =" << c << "is \"Ґ\"";
        if (c == u'Ґ') qDebug()<<"c =" << c << "is \'Ґ\'";

        if (c == "Z") qDebug()<<"c =" << c << "is \"Z\"";
        if (c == 'Z') qDebug()<<"c =" << c << "is \'Z\'";
    }
}

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
    QApplication a(argc, argv);
    f4();
}

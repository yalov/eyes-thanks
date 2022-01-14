#include <QApplication>
//#include <QDesktopWidget>
#include <QDebug>
#include <QString>
#include <QXmlStreamWriter>
#include <QFile>
#include <QTextStream>
#include <QRect>


#include <QtWidgets/QApplication>

#include "transliteration-iso9a.h"
#include "charactersets.h"
#include "global.h"

#include <stdio.h>
#include <windows.h>
#include <lm.h>

#ifndef UNICODE
#define UNICODE
#endif

//#pragma comment(lib, "Ole32.lib")
//#pragma comment(lib, "advapi32.lib")
//#pragma comment(lib, "netapi32.lib")

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <lm.h>
#include <sddl.h>


/* for ConvertSidToStringSid function */
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QTextBoundaryFinder>

void f1 () {
    QVector<QString> list = {
        QString("Афанасий Ефре'мов")                 ,
        QString("Вячеслав Коновалов")               ,
        QString("Максим Назаров")                   ,
        QString("Филипп Ґалубёў")                   ,
        QString("Толстой Кирилл Эдуардович")        ,
        QString("Караев Мстислав Евгениевич")       ,
        QString("Шепкин Марк Касьянович")           ,
        QString("Щеголяев Адриан Матвеевич")        ,
        QString("Давыдов Тимур Святославович")      ,
        QString("Разуваев Платон Елизарович")       ,
        QString("Буркин Ипатий Венедиктович")       ,
        QString("Грибков Яков Левович")             ,
        QString("Остальцев Руслан Федосиевич")      ,
        QString("Кортнев Иннокентий Викентиевич")   ,
        QString("Хоботилов Сергей Олегович")        ,
        QString("Горюшин Евгений Игнатиевич")       ,
        QString("Александров Пимен Ильевич")        ,
        QString("Кобзев Данила Елисеевич")          ,
        QString("Мозговой Осип Пахомович")          ,
        QString("Кошкин Измаил Платонович")         ,
        QString("Гадолин Виссарион Кондратович")    ,
        QString("Шостенко Ким Куприянович")         ,
        QString("Другов Эммануил Мечиславович")     ,
        QString("Бенедиктов Владимир Даниилович")   ,
        QString("Коротаев Дмитрий Тарасович")       ,
        QString("Онегин Никон Филимонович")         ,
        QString("Верясов Рюрик Казимирович")        ,
        QString("Явленский Вячеслав Никонович")     ,
        QString("Сиясинов Прохор Тихонович")        ,
        QString("Агапов Касьян Ипатиевич")          ,
        QString("Житков Леондий Валериевич")        ,
        QString("Яхин Григорий Давидович")          ,
        QString("Пушкарёв Александр Мирославович")  ,
        QString("Эссен Валентин Измаилович")        ,
        QString("Щетинин Радион Трофимович")        ,
        QString("Лясковец Данил Брониславович")     ,
        QString("Земляков Аким Ермолаевич")         ,
        QString("Милютин Даниил Сидорович")         ,
        QString("Гершкович Антип Моисеевич")        ,
        QString("Содовский Кузьма Михеевич")};

    for (auto name: list) {
        qDebug() << list.indexOf(name);
        LogToFile("names.txt", QString("%1; %2")
                  .arg(name,40)
                  .arg(transliteraction(name),40)
                  );
}
}

#ifdef false
void f2 () {
    QString str = "ЩZҐ";//u8"ЩZҐ";
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

QList<QString> f4()
{
    QList<QString> s;
    QFile file("c:/Users/User/Repositories/eyes-thanks/test.xml");
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(file.readAll());
        file.close();

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "set")
            {
                while (xml.readNextStartElement())
                {
                    if (xml.name() == "title") s.append(xml.readElementText());
                }
            }
        }
    }

    return s;
}

#endif

struct Asset
{
    QString version{};
    QList<QString> links{};
    QString toString(){
        return version +", "+ links.join(", ");
    }
};

Asset json_test(const QByteArray & json_text)
{
    Asset asset;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(json_text));

    if (jsonDocument.isNull() || jsonDocument.isEmpty() || !jsonDocument.isArray())
    {
        qDebug() << "JSON failed";
        return asset;
    }

    const QJsonArray &json = jsonDocument.array();

    for (auto json_value: json)
    {
        if (!json_value.isObject()) continue;

        auto json_ver = json_value.toObject();
        if (json_ver.contains("assets"))
        {
            auto json_assets_obj = json_ver["assets"];
            if (!json_assets_obj.isArray()) continue;
            auto json_assets = json_assets_obj.toArray();
            for (auto asset_vref: json_assets){
                if (!asset_vref.isObject()) continue;
                auto a = asset_vref.toObject();
                if (!a.contains("browser_download_url")) continue;

                asset.links.append(a["browser_download_url"].toString());

            }

            if (!asset.links.empty())
            {
                if (json_ver.contains("tag_name"))
                    asset.version = json_ver["tag_name"].toString();

                return asset;
            }
        }

    }

    return asset;
}

void f5()
{
    QFile loadFile(QStringLiteral("d:/Downloads/releases.json"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QByteArray saveData = loadFile.readAll();

    Asset a = json_test(saveData);
    qDebug() << a.toString();
}


void GraphemeCounter(QString s)
{
    qDebug()  << "String: " << s;
    qDebug()  << "Code unit count       : " << s.length();

    QTextBoundaryFinder tbf(QTextBoundaryFinder::Grapheme, s);
    int count=0;
    while(tbf.toNextBoundary()!=-1) ++count;

    qDebug() << "QTextBoundaryFinder::Grapheme count: " << count;

    qDebug() << "toUcs4 count: " << s.toUcs4().size();

    QDebug deb = qDebug();
    foreach (auto c, s.toUcs4())
        deb << QChar::fromUcs4(c);

}

int main()
{
    QString test = "a" + QString::fromUtf16(u"\U0001D161") + "a";
    GraphemeCounter(test);

    QString  a = QString::fromUtf16(u"𝕬𝕭𝕮𝕯𝕰𝕱𝕲𝕳𝕴𝕵𝕶𝕷𝕸𝕹𝕺𝕻𝕼𝕽𝕾𝕿𝖀𝖁𝖂𝖃𝖄𝖅𝖆𝖇𝖈𝖉𝖊𝖋𝖌𝖍𝖎𝖏𝖐𝖑𝖒𝖓𝖔𝖕𝖖𝖗𝖘𝖙𝖚𝖛𝖜𝖝𝖞𝖟");
    GraphemeCounter(a);


    const QString s=QString::fromUtf8(u8"abc\U00010139def\U00010102g");
    GraphemeCounter(s);
}

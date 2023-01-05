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
#include <lmaccess.h>
#include <lm.h>
#include <shellapi.h>
#include <tchar.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <lmaccess.h>
#include <windows.h>
#include <stdio.h>
#include <assert.h>
//#include <lm.h>
//#include <sddl.h>




#include <qprocess.h>
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

#ifdef false

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

#endif


void PrintWinUserName()
{
    TCHAR  infoBuf[32767];
    DWORD  bufCharCount = 32767;

    GetUserName( infoBuf, &bufCharCount );
    LPUSER_INFO_0 pBuf = nullptr;
    NetUserGetInfo(nullptr,infoBuf,2, reinterpret_cast<LPBYTE*>(&pBuf));   // infoBuf - username
    if (pBuf != nullptr)
    {
        LPUSER_INFO_2 pBuf2 = LPUSER_INFO_2(pBuf);
        QStringList full_name = QString::fromWCharArray( pBuf2->usri2_full_name ).split(" ");
        // take the largest from user names: James T.K. -> James
        std::sort(full_name.begin(), full_name.end(),[](QString& a, QString& b) { return a.size() < b.size(); } );
        //name = transliteraction(full_name.last());
        qDebug() << "full_name" << full_name.last();
        qDebug() << "infoBuf" << infoBuf;
    }
}






int main2()
{
    //QString test = "a" + QString::fromUtf16(u"\U0001D161") + "a";
    //GraphemeCounter(test);
    //QString  a = QString::fromUtf16(u"𝕬𝕭𝕮𝕯𝕰𝕱𝕲𝕳𝕴𝕵𝕶𝕷𝕸𝕹𝕺𝕻𝕼𝕽𝕾𝕿𝖀𝖁𝖂𝖃𝖄𝖅𝖆𝖇𝖈𝖉𝖊𝖋𝖌𝖍𝖎𝖏𝖐𝖑𝖒𝖓𝖔𝖕𝖖𝖗𝖘𝖙𝖚𝖛𝖜𝖝𝖞𝖟");
    //GraphemeCounter(a);
    //const QString s=QString::fromUtf8(u8"abc\U00010139def\U00010102g");
    //GraphemeCounter(s);

    //PrintWinUserName();


    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString username = env.value("USER");
    qDebug() << username;

    QString sysUsername = qEnvironmentVariable("USER");
    if (sysUsername.isEmpty()) sysUsername = qEnvironmentVariable("USERNAME");
    qDebug() << sysUsername;

    PrintWinUserName();


    // Get and display the user name.
    TCHAR  infoBuf[32767];
    DWORD  bufCharCount = 32767;

    bufCharCount = 32767;

    if(GetUserName( infoBuf, &bufCharCount ) ){
        _tprintf( TEXT("\nUser name:          %s"), infoBuf );

        qDebug() << *infoBuf;
    }

}

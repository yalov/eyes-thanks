#include "widget.h"
#include <QApplication>
#include <QLabel>
#include <QList>

QString f4()
{
    QString s = "";
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
                    if (xml.name() == "title") s = xml.readElementText();
                }
            }
        }
    }

    return s;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.show();

    QString str = f4();
    QList<QString> list{};
    QString str1{};

    qDebug() << str.size();

    for (int i=0; i<str.size();i++)
    {
        //qDebug() << c.isSurrogate();

        if (str.at(i).isSurrogate())
        {
            if (i+1<str.size() && str.at(i+1).isSurrogate())
            {
                list.append(str.mid(i,2));
                str1 += str.mid(i,2) + "\n";
                i++;
                continue;
            }
        }
        else
        {
            list.append(str.at(i));
            str1 = (str1 + str.at(i)) + "\n";
        }
    }


    QLabel * label = new QLabel();
    label->setText(str1);
    label->show();
    return a.exec();
}

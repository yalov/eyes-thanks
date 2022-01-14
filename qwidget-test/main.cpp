#include "widget.h"
#include <QApplication>
#include <QLabel>
#include <QList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    //QString str = f4();
    //QLabel * label = new QLabel();
    //label->setText("aaaaaaaaaaaaaa");
    //label->show();
    return a.exec();
}

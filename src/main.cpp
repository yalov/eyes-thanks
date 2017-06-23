//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "trayicon.h"
#include <QApplication>
//QT_NO_DEBUG_OUTPUT

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);

    TrayIcon w;
    w.show();

    return a.exec();
}

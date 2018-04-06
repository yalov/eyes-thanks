//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "trayicon.h"
#include <QApplication>
#include <iostream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qSetMessagePattern("[%{time h:mm:ss.zzz}] %{message}");

    a.setQuitOnLastWindowClosed(false);

    TrayIcon w;
    w.show();

    return a.exec();
}

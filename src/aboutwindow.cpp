//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//                                                                                  //
//      This file is part of Eyes' Thanks.                                          //
//                                                                                  //
//      Eyes' Thanks is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License either                 //
//      version 3 of the License, or (at your option) any later version.            //
//                                                                                  //
//      Eyes' Thanks is distributed in the hope that it will be useful,             //
//      but WITHOUT ANY WARRANTY; without even the implied warranty of              //
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               //
//      GNU General Public License for more details.                                //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with Eyes' Thanks.  If not, see <http://www.gnu.org/licenses/>.       //
//----------------------------------------------------------------------------------//

#include "aboutwindow.h"

#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent)
{
    QString win_title = tr("About Eyes' Thanks");
    QString app_name = "Eyes' Thanks"; //QString(APP_NAME);
    QString app_version = QString(APP_VERSION);
    QString dev_name = QString(DEVELOP_NAME);
    QString dev_email = "alexander.yalov@gmail.com";
    QString bitbucket_path = "https://bitbucket.org/flart/eyes-thanks";
    QString license_path = "http://www.gnu.org/licenses/gpl-3.0.html";
    QString license_name = "GNU GPLv3";

#ifdef _WIN32
    QIcon aboutIcon     = QIcon(":icons/help-about.png");
    setWindowFlags( Qt::WindowTitleHint | Qt::CustomizeWindowHint);
#else
    QIcon aboutIcon     = QIcon::fromTheme("help-about");       //QStyle::SP_MessageBoxInformation
#endif
    setWindowIcon(aboutIcon);

    setWindowTitle(win_title);


    QVBoxLayout * TopLayout = new QVBoxLayout(this);

    QPushButton * ok = new QPushButton("OK");
    connect(ok, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout * buttons = new QHBoxLayout();
    buttons->addStretch();
    buttons->addWidget(ok);

    QTabWidget * tabs = new QTabWidget;
    QHBoxLayout * middleLayout = new QHBoxLayout();
    middleLayout->addWidget(tabs);

    TopLayout->addWidget(new QLabel("<h1 align = center>" + app_name +" v" + app_version + "</h1>"));
    TopLayout->addLayout(middleLayout);
    TopLayout->addLayout(buttons);

    {
        QWidget * tab1 = new QWidget;

        tabs->addTab(tab1, QIcon(), tr("About"));

        QHBoxLayout * lay1 = new QHBoxLayout(tab1);
        QLabel* pic = new QLabel();
        pic->setPixmap(QPixmap(":/icons/00.png").scaledToWidth(128));
        pic->setFixedSize(pic->sizeHint());

        pic->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);


        QString aboutText = QString(
            "<font size=4"
                "<p align = justify>" +
                tr("Eyes' Thanks is a software that protect your eyes. "
                   "The program frequently alerts you to take rest breaks by showing fullscreen image (random from the folder). "
                   "It supports multiple monitor system and wide image for all monitors.") +
                "</p>"
                "<br>"
                "<p align = center>" +
                    tr("Programmed in %1, based on Qt %2 toolkit %3") +
                "</p>"
                "<p align = center>" +
                    tr("Home Page")  +": <a href=\"%4\">%4</a>"
                "</p>"
                "<p align = center>"
                    "Copyright © 2015, License: <a href=\"%5\">%6</a>"  "<br>"
                    "%7 <a href='mailto:%8'>%8</a>"
                "</p>"
            "</font>"
            ).arg("<nobr>C++</nobr>", "<nobr>" QT_VERSION_STR "</nobr>" ,CompilerInfo(),
                  bitbucket_path, license_path, license_name, dev_name, dev_email);


        QLabel * lb_aboutText = new QLabel(aboutText);
        lb_aboutText->setWordWrap(true);
        lb_aboutText->setTextFormat(Qt::RichText);
        lb_aboutText->setTextInteractionFlags(Qt::TextBrowserInteraction);
        lb_aboutText->setOpenExternalLinks(true);
        lay1->addWidget(pic);
        lay1->addSpacing(25);
        lay1->addWidget(lb_aboutText);
    }

    QFile file(":/copying.html");
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QWidget * tab2 = new QWidget;
        tabs->addTab(tab2, QIcon(), tr("License"));

        QVBoxLayout * lay2 = new QVBoxLayout(tab2);

        QTextBrowser * textBrowser = new QTextBrowser();

        QTextStream ReadFile(&file);
        textBrowser->setText(ReadFile.readAll());
        textBrowser->setOpenExternalLinks(true);
        lay2->addWidget(textBrowser);
    }
    file.close();
}

QString AboutWindow::CompilerInfo()
{
    QString compiler_info = QString("<nobr>(");

//#ifdef _WIN32
//    compiler_info += "Win ";
//#endif

//#ifdef __linux__
//    compiler_info += "Linux ";
//#endif

#ifdef __GNUC__

#ifdef __MINGW32__
    compiler_info += "MinGW ";
#else
    compiler_info += "GCC ";
#endif

    compiler_info += QString("%1.%2.%3,").arg( __GNUC__).arg( __GNUC_MINOR__).arg( __GNUC_PATCHLEVEL__);
#endif

#ifdef _MSC_VER
    version += "MSVC++ ";
    switch (_MSC_VER)
    {
        case 1900: compiler_info+= "14.0"; break;
        case 1800: compiler_info+= "12.0"; break;
        case 1700: compiler_info+= "11.0"; break;
        case 1600: compiler_info+= "10.0"; break;
        case 1500: compiler_info+= "9.0" ; break;
        case 1400: compiler_info+= "8.0" ; break;
        default  : compiler_info+= "<8.0"; break;
    }

    compiler_info += ",";

#endif

    switch (sizeof(void*))
    {
        case 8:  compiler_info += " x64"; break;
        case 4:  compiler_info += " x86"; break;

        default: compiler_info += " ORLY?"; break;
    }

    compiler_info += ")</nobr>";
    return compiler_info;
}

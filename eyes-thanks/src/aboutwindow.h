//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QTextStream>
#include "QtNetwork/QNetworkAccessManager"
#include <QSslSocket>

class AboutWindow : public QDialog
{
    Q_OBJECT
public:
    AboutWindow(QWidget *parent = 0): QDialog(parent)
    {
        const QString win_title = tr("About Eyes' Thanks");
        const QString app_name = QString(APP_NAME);
        const QString app_version = QString(APP_VERSION);
        const QString dev_name = QString(DEV_NAME);
        const QString dev_email = QString(DEV_EMAIL);
        const QString repository_url = QString(REPO_URL);
        const QString license_url = "http://www.gnu.org/licenses/gpl-3.0.html";
        const QString license_path = ":/copying.html";
        const QString license_name = "GNU GPLv3";
        const QString logo_path = ":icons/logo.png";
        const QString build_year = __DATE__ + 7;
        const QString Qt_version_compiletime = QString(QT_VERSION_STR);
        const QString Qt_version_runtime = qVersion();

        QRegularExpression re("\\S+\\s+\\d+\\.\\d+\\.\\S+");

        QString OpenSSL_version_compile = re.match(QSslSocket::sslLibraryBuildVersionString()).captured();
        QString OpenSSL_version_run = re.match(QSslSocket::sslLibraryVersionString()).captured();

        if (OpenSSL_version_run == "")
            OpenSSL_version_run = "OpenSSL dlls are missing";


#ifdef _WIN32
        QIcon aboutIcon     = QIcon(":icons/help-about.png");
        setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
#else
        QIcon aboutIcon     = QIcon::fromTheme("help-about");       //QStyle::SP_MessageBoxInformation
#endif
        setWindowIcon(aboutIcon);
        setWindowTitle(win_title);

        QVBoxLayout *TopLayout = new QVBoxLayout(this);

        QPushButton *ok = new QPushButton("OK");
        connect(ok, SIGNAL(clicked()), this, SLOT(close()));

        QHBoxLayout *buttons = new QHBoxLayout();
        buttons->addStretch();
        buttons->addWidget(ok);

        QTabWidget *tabs = new QTabWidget;
        QHBoxLayout *middleLayout = new QHBoxLayout();
        middleLayout->addWidget(tabs);

        TopLayout->addWidget(new QLabel("<h1 align = center>" + app_name + " v" + app_version + "</h1>"));
        TopLayout->addLayout(middleLayout);
        TopLayout->addLayout(buttons);

        {
            QWidget *tab1 = new QWidget;

            tabs->addTab(tab1, QIcon(), tr("About"));

            QHBoxLayout *lay1 = new QHBoxLayout(tab1);
            QLabel *pic = new QLabel();
            pic->setPixmap(QPixmap(logo_path).scaledToWidth(128));
            pic->setFixedSize(pic->sizeHint());
            pic->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

            QString aboutText =
                QString(
                    "<font size=4>"
                    "<p align = justify>" +
                    tr("Eyes' Thanks is a software that protect your eyes. "
                       "The program frequently alerts you to take rest breaks by showing fullscreen image (random from the folder). "
                       "It supports multiple monitor system and wide image for all monitors.") +
                    "<br></p>") +
                QString(
                    "<p align = center>"
                    "Compile time:<br>Qt %1, %2"
                    "</p>"

                    "<p align = center>"
                    "Runtime:<br>Qt %4 (%3 inside),<br>%5"
                    "</p>"
                ).arg("<nobr>" + Qt_version_compiletime + "</nobr>", CompilerInfo(),
                      "<nobr>" +  OpenSSL_version_compile + "</nobr>", "<nobr>" + Qt_version_runtime + "</nobr>",
                      "<nobr>" + OpenSSL_version_run + "</nobr>") +

                QString(
                    "<p align = center>"
                    "Repository"  ": <a href=\"%1\">%1</a>"
                    "</p>"
                    "<p align = center>"
                    "Copyright © %2 License: <a href=\"%3\">%4</a>"  "<br>"
                    "%5 <a href='mailto:%6'>%6</a>"
                    "</p>"
                    "</font>"
                ).arg(repository_url, build_year, license_url, license_name, dev_name, dev_email);


            QLabel *lb_aboutText = new QLabel(aboutText);
            lb_aboutText->setWordWrap(true);
            lb_aboutText->setTextFormat(Qt::RichText);
            lb_aboutText->setTextInteractionFlags(Qt::TextBrowserInteraction);
            lb_aboutText->setOpenExternalLinks(true);
            lay1->addWidget(pic);
            lay1->addSpacing(25);
            lay1->addWidget(lb_aboutText);
        }

        QFile file(license_path);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QWidget *tab2 = new QWidget;
            tabs->addTab(tab2, QIcon(), tr("License"));

            QVBoxLayout *lay2 = new QVBoxLayout(tab2);

            QTextBrowser *textBrowser = new QTextBrowser();

            QTextStream ReadFile(&file);
            textBrowser->setText(ReadFile.readAll());
            textBrowser->setOpenExternalLinks(true);
            lay2->addWidget(textBrowser);
        }
        file.close();
    }

private:
    QString CompilerInfo()
    {
        QString compiler_info = QString("<nobr>");

#ifdef __GNUC__
#ifdef __MINGW32__
        compiler_info += "MinGW ";
#else
        compiler_info += "GCC ";
#endif
        compiler_info += QString("%1.%2.%3,").arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
#endif

#ifdef _MSC_VER
        compiler_info += "MSVC++ ";
        if (_MSC_VER >= 2000) compiler_info += "14.0+";
        else if (_MSC_VER >= 1900) compiler_info += "14.0"; // VS 2015
        else if (_MSC_VER >= 1800) compiler_info += "12.0";
        else if (_MSC_VER >= 1700) compiler_info += "11.0";
        else if (_MSC_VER >= 1600) compiler_info += "10.0";
        else if (_MSC_VER >= 1500) compiler_info += "9.0";
        else if (_MSC_VER >= 1400) compiler_info += "8.0";
        else                      compiler_info += "< 8.0";

        compiler_info += ",";

#endif

        switch (sizeof(void *)) {
        case 8:
            compiler_info += " x64";
            break;
        case 4:
            compiler_info += " x86";
            break;

        default:
            compiler_info += " ORLY?";
            break;
        }

        compiler_info += "</nobr>";
        return compiler_info;
    }
};

#endif // ABOUT_H

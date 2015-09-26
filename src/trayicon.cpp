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

#include "trayicon.h"

#include <QMenu>
#include <QMessageBox>

TrayIcon::TrayIcon(QWidget *parent)
    : QSystemTrayIcon(parent),
      TrayMessageShowed(false), TimeRemains(""), ActualIconRatio(-1)
{

    QIcon testIcon    = QIcon(":icons/00.png");

#ifdef _WIN32
    QIcon settingIcon = QIcon(":icons/configure.png");
    QIcon pauseIcon   = QIcon(":icons/media-playback-pause.png");
    QIcon aboutIcon   = QIcon(":icons/help-about.png");
    QIcon quitIcon    = QIcon(":icons/application-exit.png");

#else
    QIcon settingIcon = QIcon::fromTheme("document-properties");//QIcon::fromTheme( "configure" );
    QIcon pauseIcon   = QIcon::fromTheme("media-playback-pause");//QWidget().style()->standardIcon(QStyle::SP_MediaPause);
    QIcon aboutIcon   = QIcon::fromTheme("help-about");//QWidget().style()->standardIcon(QStyle::SP_MessageBoxInformation);
    QIcon quitIcon    = QIcon::fromTheme("application-exit");//QWidget().style()->standardIcon(QStyle::SP_DialogCloseButton);

#endif

    TestAct        = new QAction(testIcon, tr("Test"), this);
    connect(TestAct, SIGNAL(triggered()), this, SLOT(ShowView()));

    ShowSettingAct = new QAction(settingIcon, tr("Setting"), this);
    connect(ShowSettingAct, SIGNAL(triggered()), this, SLOT(ShowDialog()));

    PauseAct       = new QAction(pauseIcon, tr("Pause"), this);
    connect(PauseAct, SIGNAL(triggered()), this, SLOT(Pause()));

    QuitAct        = new QAction(quitIcon, tr("Quit"), this);
    connect(QuitAct, SIGNAL(triggered()), this, SLOT(Quit()));

    AboutAct       = new QAction(aboutIcon, tr("About"), this);
    connect(AboutAct, SIGNAL(triggered()), this, SLOT(About()));

    QMenu * ContextMenu = new QMenu();
    ContextMenu->addAction(TestAct);
    ContextMenu->addAction(ShowSettingAct);
    ContextMenu->addAction(PauseAct);
    ContextMenu->addAction(AboutAct);
    ContextMenu->addAction(QuitAct);

    setContextMenu(ContextMenu);
    setIcon( QIcon(":/icons/00.png"));
    setToolTip(QString(tr("Until break") + " %1").arg(TimeRemains));

    QFontDatabase::addApplicationFont(":fonts/TrixiePro-Heavy.otf");


    ReadSettings();

    view = new View();
    ViewTimer = new Timer(UPDATE_PERIOD_1, pauseContinuous);
    DialogTimer = new Timer(UPDATE_PERIOD_2, pauseInterval);
    DialogTimer->start();

    connect(DialogTimer, SIGNAL(finished()), this, SLOT(RefreshmentStart()));
    connect(ViewTimer, SIGNAL(update()), this, SLOT(ViewUpdateTime()));
    connect(DialogTimer, SIGNAL(update()), this, SLOT(DialogUpdateTime()));

    connect(view,      SIGNAL(view_close()),    ViewTimer, SLOT(stop()));
    connect(ViewTimer, SIGNAL(finished()), ViewTimer, SLOT(stop()));
    connect(ViewTimer, SIGNAL(finished()), view, SLOT(close()));



    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(Activated(QSystemTrayIcon::ActivationReason)));


    if (Counter == 0)
    {
        ShowDialog();
    }
}

TrayIcon::~TrayIcon()
{

}


void TrayIcon::ReadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setIniCodec("utf8");

    settings.beginGroup("Settings");
    Counter = settings.value("Counter",0).toInt();
    pauseInterval = settings.value("Interval",60*60*1000).toInt();
    pauseContinuous = settings.value("Continuous",60*1000).toInt();

    ImagesPath = settings.value("background_path", "").toString();

    isClock = settings.value("clock_enabled", true).toBool();
    isMessage30sec = settings.value("message_enabled", true).toBool();
    isDebug = settings.value("debug_enabled", false).toBool();
    isText = settings.value("text_enabled", false).toBool();

    imageAspectMode = settings.value("aspectMode", tr("Auto")).toString();
    Text = settings.value("text", tr("All work and no play\nmakes Jack a dull boy.")).toString();
    settings.endGroup();
}

void TrayIcon::WriteSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setIniCodec("utf8");

    settings.beginGroup("Settings");
    settings.setValue("Counter", ++Counter);
    settings.setValue("Interval", pauseInterval);
    settings.setValue("Continuous", pauseContinuous);
    settings.setValue("background_path", ImagesPath);
    settings.setValue("clock_enabled", isClock);
    settings.setValue("message_enabled", isMessage30sec);
    settings.setValue("debug_enabled", isDebug);
    settings.setValue("aspectMode", imageAspectMode);

    settings.setValue("text_enabled", isText);
    settings.setValue("text", Text);
    settings.endGroup();
}



void TrayIcon::CloseDialog(bool minimized)
{
    if (!minimized)
        showMessage(tr("Eyes' Thanks"),
                             tr("The program will keep running in the system tray. To terminate the program, "
                                "choose \"Quit\" in the context menu of the system tray entry."));

    ShowSettingAct->setEnabled(true);
}


void TrayIcon::Save(int pauseinterval, int pausecontinuous, QString Imagespath, QString imageaspectMode,
                    bool isdebug, bool istext, bool isclock, bool ismessage30sec, QString text)
{
    pauseInterval = pauseinterval;
    pauseContinuous = pausecontinuous;
    ImagesPath = Imagespath;
    imageAspectMode = imageaspectMode;
    isDebug = isdebug;
    isText = istext;
    isClock = isclock;
    isMessage30sec = ismessage30sec;
    Text = text;


    WriteSettings();

    PauseAct->setText(tr("Pause"));
#ifdef _WIN32
    PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
#else
    PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif
    DialogTimer->restart(UPDATE_PERIOD_2, pauseInterval );
    ViewTimer->Init(UPDATE_PERIOD_1, pauseContinuous);

    TrayMessageShowed = false;
    ActualIconRatio = -1;
}

void TrayIcon::Quit()
{
    WriteSettings();
    qApp->quit();
}

void TrayIcon::About()
{
    AboutWindow * about = new AboutWindow();
    about->show();
}


void TrayIcon::Pause()
{
    if (DialogTimer->isActive)
    {
        PauseAct->setText(tr("Unpause"));

    #ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-start.png"));
    #else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-start"));
    #endif

        DialogTimer->pause();

        setIcon( QIcon(":/icons/--.png"));
        setToolTip(toolTip() + " (" + tr("Pause") + ")");
        emit updateLabel(TimeRemains +  " (" + tr("Pause")+")");
    }
    else
    {
        PauseAct->setText(tr("Pause"));
    #ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
    #else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
    #endif

        DialogTimer->unpause();

        double ratio = DialogTimer->ratio();

        if      (ratio <0.25) setIcon( QIcon(":/icons/00.png"));
        else if (ratio <0.50) setIcon( QIcon(":/icons/25.png"));
        else if (ratio <0.75) setIcon( QIcon(":/icons/50.png"));
        else if (ratio <0.87) setIcon( QIcon(":/icons/75.png"));
        else if (ratio <0.95) setIcon( QIcon(":/icons/87.png"));
        else                  setIcon( QIcon(":/icons/95.png"));
    }
}


void TrayIcon::Activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
#ifdef __linux__
    case QSystemTrayIcon::Trigger:
        this->ShowDialog();
        break;
#endif

    case QSystemTrayIcon::DoubleClick:
        this->ShowDialog();
        break;

    case QSystemTrayIcon::MiddleClick:
        this->Pause();
        break;
    default:
        ;
    }
}



void TrayIcon::ShowDialog()
{
    dialog = new Dialog();
    connect(dialog, SIGNAL(closedialog(bool)), this, SLOT(CloseDialog(bool)));
    connect(dialog, SIGNAL(save(int, int, QString, QString, bool, bool, bool, bool, QString)),
            this, SLOT(Save(int, int, QString, QString, bool, bool, bool, bool, QString)));

    connect(this, SIGNAL(updateLabel(QString)), dialog, SLOT(UpdateLabel(QString)));

    ShowSettingAct->setEnabled(false);

    dialog->SetValues(pauseInterval, pauseContinuous, ImagesPath, imageAspectMode,
                                 isDebug, isText, isClock, isMessage30sec, Text, Counter);

#ifdef __linux__
    dialog->showNormal();
#else
    dialog->setWindowState((dialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    dialog->show();
#endif
}



void TrayIcon::DialogUpdateTime()
{
    qint64 remains =  DialogTimer->remains();;
    TimeRemains = DialogTimer->remains_str(false);


    emit updateLabel(TimeRemains);

    setToolTip(QString(tr("Until break") +" %1 "+tr("sec")).arg(TimeRemains));

    if (isMessage30sec && remains < 30000 && !TrayMessageShowed)
    {
        showMessage(tr("Eyes' Thanks"),QString(tr("Until break") +" %1 "+tr("sec")).arg(qRound( remains/1000.)));
        TrayMessageShowed = true;
    }


    double ratio = DialogTimer->ratio();

    if (ActualIconRatio == -1)
    {
        setIcon( QIcon(":/icons/00.png"));
        ActualIconRatio = 0;
    }
    else if (ratio >0.25 && ActualIconRatio == 0)
    {
        setIcon( QIcon(":/icons/25.png"));
        ActualIconRatio = 0.25;
    }
    else if (ratio >0.50 && ActualIconRatio == 0.25 )
    {
        setIcon( QIcon(":/icons/50.png"));
        ActualIconRatio = 0.5;
    }
    else if (ratio >0.75&& ActualIconRatio == 0.5 )
    {
        setIcon( QIcon(":/icons/75.png"));
        ActualIconRatio = 0.75;
    }
    else if (ratio >0.87 && ActualIconRatio == 0.75 )
    {
        setIcon( QIcon(":/icons/87.png"));
        ActualIconRatio = 0.87;
    }
    else if (ratio >0.95 && ActualIconRatio == 0.87 )
    {
        setIcon( QIcon(":/icons/95.png"));
        ActualIconRatio = 0.95;
    }
}


void TrayIcon::RefreshmentStart()
{
    ShowView();
    DialogTimer->restart();
    TrayMessageShowed = false;
    ActualIconRatio = -1;
    ViewTimer->start();
}


void TrayIcon::ShowView()
{
    QStringList images = QDir( ImagesPath).entryList(QStringList() <<"*.jpg" << "*.png");

    QString pic_path;
    if (!images.isEmpty())
    {
        pic_path = ImagesPath + "/" + images[qrand()%images.size()];
    }


    QString clock;
    if (isClock)  clock = QTime::currentTime().toString("hh:mm");
    else          clock = "";


    QString text;
    if (isText)    text = Text;
    else           text = "";

    text.replace("%interval", QString::number( pauseInterval / 1000/60) + " " + tr("min"));
    text.replace("%break", QString::number(pauseContinuous/1000) + " " + tr("sec"));

    view->ShowRefreshment(pic_path, clock, text, isDebug, imageAspectMode);


    //QLabel *l = new QLabel("Test");

   // l->setGeometry(QRect( 0, 0,3840,1080));
   // l->show();
//    Qt::FramelessWindowHint

}



void TrayIcon::ViewUpdateTime()
{
    if (view->isHidden())
    {
        ViewTimer->stop();
        return;
    }

    view->UpdateValues(ViewTimer->remains_str(false), ViewTimer->ratio());
}

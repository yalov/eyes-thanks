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

TrayIcon::TrayIcon(QWidget *parent): QSystemTrayIcon(parent),
    dialog(NULL), TrayMessageShowed(false), TimeRemains(""), CurrentIconRatio(-1)
{
    LangPath = "languages";
    Translator = new QTranslator();

    createLangActionGroup();

    createActions();
    createContextMenu();


    ReadSettings();

    for (auto f : QDir(":fonts").entryInfoList())
        QFontDatabase::addApplicationFont(f.absoluteFilePath());
        //UKIJ Diwani Yantu

    InitIcons();

    setCurrentIcon(0.0);

    ViewTimer = new Timer(UPDATE_PERIOD_1, pauseContinuous);
    DialogTimer = new Timer(UPDATE_PERIOD_2, pauseInterval, isLogging);
    DialogTimer->start();

    connect(ViewTimer, SIGNAL(update()), this, SLOT(ViewUpdateTime()));
    connect(DialogTimer, SIGNAL(update()), this, SLOT(DialogUpdateTime()));
    connect(DialogTimer, SIGNAL(finished()), this, SLOT(RefreshmentStart()));
    connect(ViewTimer, SIGNAL(finished()), ViewTimer, SLOT(stop()));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(Activated(QSystemTrayIcon::ActivationReason)));


    if (Counter == 0) ShowDialog();

}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::createLangActionGroup()
{
    LangActGroup = new QActionGroup(this);
    LangActGroup->setExclusive(true); // Mutually exclusive checking
    connect(LangActGroup, SIGNAL (triggered(QAction *)), this, SLOT (slotLanguageChanged(QAction *)));

    QDir dir(LangPath);
    QStringList fileNames = dir.entryList(QStringList("lang_*.qm"));

    bool haveEnglish_qm = false;

    for (auto fileName: fileNames) // "lang_ru.qm"
    {
        QTranslator translator;
        translator.load(fileName, LangPath);

        auto locale = fileName.split('.').first().split('_').last();  // ru

        if (locale == "en") haveEnglish_qm = true;

        QString lang = translator.translate("TrayIcon", "English");

        QAction *action = new QAction(lang, this);
        action->setCheckable(true);
        action->setData(locale);

        LangActGroup->addAction(action);
    }

    if (!haveEnglish_qm)
    {
        QString lang = "English";
        QAction *action = new QAction(lang, this);
        action->setCheckable(true);
        //action->setChecked(true);
        action->setData("en");
        LangActGroup->addAction(action);
    }
}

void TrayIcon::createActions()
{
#ifdef _WIN32

    QIcon testIcon    = QIcon(":icons/video-display.png");
    QIcon settingIcon = QIcon(":icons/configure.png");
    QIcon pauseIcon   = QIcon(":icons/media-playback-pause.png");
    QIcon updateIcon  = QIcon(":icons/system-software-update");
    QIcon aboutIcon   = QIcon(":icons/help-about.png");
    QIcon quitIcon    = QIcon(":icons/application-exit.png");

#else
    QIcon testIcon    = QIcon::fromTheme("video-display"); // preferences-desktop-display
    QIcon settingIcon = QIcon::fromTheme("document-properties");//QIcon::fromTheme( "configure" );
    QIcon pauseIcon   = QIcon::fromTheme("media-playback-pause");//QWidget().style()->standardIcon(QStyle::SP_MediaPause);
    QIcon updateIcon  = QIcon::fromTheme("system-software-update");
    QIcon aboutIcon   = QIcon::fromTheme("help-about");//QWidget().style()->standardIcon(QStyle::SP_MessageBoxInformation);
    QIcon quitIcon    = QIcon::fromTheme("application-exit");//QWidget().style()->standardIcon(QStyle::SP_DialogCloseButton);

#endif

    TestAct        = new QAction(testIcon, tr("Test"), this);
    connect(TestAct, SIGNAL(triggered()), this, SLOT(ShowView()));

    ShowSettingAct = new QAction(settingIcon, tr("Setting"), this);
    connect(ShowSettingAct, SIGNAL(triggered()), this, SLOT(ShowDialog()));

    PauseAct       = new QAction(pauseIcon, tr("Pause"), this);
    connect(PauseAct, SIGNAL(triggered()), this, SLOT(Pause()));

    UpdaterAct       = new UpdateAction(updateIcon, tr("Check for Updates"), this);

    AboutAct       = new QAction(aboutIcon, tr("About"), this);
    connect(AboutAct, SIGNAL(triggered()), this, SLOT(About()));

    QuitAct        = new QAction(quitIcon, tr("Quit"), this);
    connect(QuitAct, SIGNAL(triggered()), this, SLOT(Quit()));
}

void TrayIcon::createContextMenu()
{
    QMenu *ContextMenu = new QMenu();
    ContextMenu->addAction(TestAct);
    ContextMenu->addAction(ShowSettingAct);
    ContextMenu->addAction(PauseAct);

    SubMenuLanguages = new QMenu(tr("Languages"));
    SubMenuLanguages->addActions(LangActGroup->actions());
    ContextMenu->addMenu(SubMenuLanguages);

    ContextMenu->addAction(UpdaterAct);
    ContextMenu->addAction(AboutAct);
    ContextMenu->addAction(QuitAct);

    setContextMenu(ContextMenu);
}

void TrayIcon::ReadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setIniCodec("utf8");

    settings.beginGroup("Settings");
    LoadLanguage(settings.value("lang", QLocale::system().name().split('_').front()).toString());
    Counter         = settings.value("counter", 0).toInt();
    pauseInterval   = settings.value("interval", 60 * 60 * 1000).toInt();
    pauseContinuous = settings.value("continuous", 60 * 1000).toInt();

    ImagesPath      = settings.value("background_path", "").toString();
    ImagesPath_alt  = settings.value("background_path_alt", "").toString();


    isClock         = settings.value("clock_enabled", true).toBool();
    isMessage30sec  = settings.value("message_enabled", true).toBool();
    isLogging       = settings.value("logging_enabled", false).toBool();
    isText          = settings.value("text_enabled", false).toBool();
    isPrettyFont    = settings.value("prettyFont_enabled", true).toBool();

    bool defaultisStartupLink;
    if (QFile::exists(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                      QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk"))
        defaultisStartupLink = true;
    else
        defaultisStartupLink = false;

    isStartupLink   = settings.value("startupLink_enabled", defaultisStartupLink).toBool();

    imageAspectMode = settings.value("aspectMode", qApp->translate("App","Auto")).toString();
    Text            = settings.value("text", tr("All work and no play\nmakes Jack a dull boy.")).toString();

    iconsMode       = static_cast<IconsMode>(settings.value("iconsMode", IconsMode::light).toUInt());
    settings.endGroup();
}

void TrayIcon::WriteSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setIniCodec("utf8");

    settings.beginGroup("Settings");
    settings.setValue("lang", LangCurrent);
    settings.setValue("counter", Counter);
    settings.setValue("interval", pauseInterval);
    settings.setValue("continuous", pauseContinuous);
    settings.setValue("background_path", ImagesPath);
    settings.setValue("background_path_alt", ImagesPath_alt);

    settings.setValue("clock_enabled", isClock);
    settings.setValue("message_enabled", isMessage30sec);
    settings.setValue("logging_enabled", isLogging);
    settings.setValue("prettyFont_enabled", isPrettyFont);
    settings.setValue("startupLink_enabled", isStartupLink);
    settings.setValue("aspectMode", imageAspectMode);

    settings.setValue("text_enabled", isText);
    settings.setValue("text", Text);

    settings.setValue("iconsMode", static_cast<uint>(iconsMode));


    settings.endGroup();
}



void TrayIcon::CloseDialog()
{

    ShowSettingAct->setEnabled(true);
    //dialog->closedialog();
    //dialog->close();
    dialog = NULL;

}


void TrayIcon::Save(int pauseinterval, int pausecontinuous, QString Imagespath, QString Imagespath_alt, QString imageaspectMode,
                    bool islogging, bool istext, bool isclock, bool ismessage30sec, bool isprettyFont, bool isstartuplink, QString text, IconsMode iconsmode)
{

    // create and delete StartUp Icon, at save time.
    if (isstartuplink != isStartupLink) {
        QString startup_lnk =  QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk";

        if (isstartuplink == true)
            QFile::link(QApplication::applicationFilePath(), startup_lnk);
        else
            QFile::remove(startup_lnk);

    }

    pauseInterval = pauseinterval;
    pauseContinuous = pausecontinuous;
    ImagesPath = Imagespath;
    ImagesPath_alt = Imagespath_alt;
    imageAspectMode = imageaspectMode;
    isLogging = islogging;
    isText = istext;
    isClock = isclock;
    isMessage30sec = ismessage30sec;
    isPrettyFont = isprettyFont;
    isStartupLink = isstartuplink;
    Text = text;
    iconsMode = iconsmode;
    CurrentIconRatio = -1;

    WriteSettings();


    InitIcons();
    setCurrentIcon(DialogTimer->ratio());

    PauseAct->setText(tr("Pause"));
#ifdef _WIN32
    PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
#else
    PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif
    DialogTimer->restart(UPDATE_PERIOD_2, pauseInterval);
    ViewTimer->Init(UPDATE_PERIOD_1, pauseContinuous);

    TrayMessageShowed = false;

    emit updateLabel(DialogTimer->remains_str(false), DialogTimer->ratio());
}

void TrayIcon::Quit()
{
    Counter++;
    WriteSettings();
    qApp->quit();
}

void TrayIcon::About()
{
    AboutWindow *about = new AboutWindow();
    about->show();
}

void TrayIcon::Pause()
{
    if (DialogTimer->isActive) {
        PauseAct->setText(tr("Unpause"));

#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-start.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-start"));
#endif

        DialogTimer->pause();

        setPauseIcon();
        setToolTip(toolTip() + " (" + tr("Pause") + ")");
        emit updateLabel(DialogTimer->remains_str(false) +  " (" + tr("Pause") + ")", -DialogTimer->ratio());
    } else {
        PauseAct->setText(tr("Pause"));
#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif

        DialogTimer->unpause();

        setCurrentIconbyCurrentIconRatio();

        emit updateLabel(DialogTimer->remains_str(false), DialogTimer->ratio());
    }
}

void TrayIcon::Activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (!dialog) this->ShowDialog();
        else dialog->close();
        break;

    case QSystemTrayIcon::MiddleClick:
        this->Pause();
        break;
    default:
        ;
    }
}

void TrayIcon::setPauseIcon()
{
    setIcon(ipp);
}

void TrayIcon::setCurrentIconbyCurrentIconRatio()
{
    if (CurrentIconRatio == 0.25) setIcon(i25);
    else if (CurrentIconRatio == 0.5) setIcon(i50);
    else if (CurrentIconRatio == 0.75) setIcon(i75);
    else if (CurrentIconRatio == 0.87) setIcon(i87);
    else if (CurrentIconRatio == 0.95) setIcon(i95);
    else                                setIcon(i00);  //if CurrentIconRatio == 0 or -1
}

//-----------------------------------------------------------------
// Called every time, when a menu entry of the language menu is called
void TrayIcon::slotLanguageChanged(QAction* action)
{
    if(0 != action)
    {
        // load the language dependant on the action content
        LoadLanguage(action->data().toString());
        WriteSettings();
    }
}

// Called by ReadSetting
//-----------------------------------------------------------------
void TrayIcon::LoadLanguage(const QString& rLanguage)
{
    if(LangCurrent != rLanguage && !rLanguage.isEmpty())
    {
        LangCurrent = rLanguage;
        QLocale locale = QLocale(LangCurrent);
        QLocale::setDefault(locale);

        qApp->removeTranslator(Translator);

        bool load = Translator->load(QString("lang_%1.qm").arg(LangCurrent), LangPath);
        if (load){
            qApp->installTranslator(Translator);
        }

        retranslate();

        if (LangActGroup)
            for (auto action:LangActGroup->actions())
            {
                if (action->data().toString() == LangCurrent)
                    action->setChecked(true);
            }
    }
}


//-----------------------------------------------------------------
void TrayIcon::retranslate()
{
    TestAct->setText(tr("Test"));
    ShowSettingAct->setText(tr("Setting"));
    PauseAct->setText(tr("Pause"));
    QuitAct->setText(tr("Quit"));
    AboutAct->setText(tr("About"));
    UpdaterAct->setText(tr("Check for Updates"));
    SubMenuLanguages->setTitle(tr("Languages"));
    //view->ButtonText->setPlainText(view->tr("Close"));

    if(dialog)
        dialog->retranslate();
}


//-----------------------------------------------------------------
void TrayIcon::setCurrentIcon(double ratio)
{
    if (CurrentIconRatio == -1) {
        setIcon(i00);
        CurrentIconRatio = 0;
    } else if (ratio > 0.25 && CurrentIconRatio == 0) {
        setIcon(i25);
        CurrentIconRatio = 0.25;
    } else if (ratio > 0.50 && CurrentIconRatio == 0.25) {
        setIcon(i50);
        CurrentIconRatio = 0.5;
    } else if (ratio > 0.75 && CurrentIconRatio == 0.5) {
        setIcon(i75);
        CurrentIconRatio = 0.75;
    } else if (ratio > 0.87 && CurrentIconRatio == 0.75) {
        setIcon(i87);
        CurrentIconRatio = 0.87;
    } else if (ratio > 0.95 && CurrentIconRatio == 0.87) {
        setIcon(i95);
        CurrentIconRatio = 0.95;
    }
}

void TrayIcon::InitIcons()
{

    if (iconsMode == IconsMode::light) {
        ipp = QIcon(":icons/light/pp.png");
        i00 = QIcon(":icons/light/00.png");
        i25 = QIcon(":icons/light/25.png");
        i50 = QIcon(":icons/light/50.png");
        i75 = QIcon(":icons/light/75.png");
        i87 = QIcon(":icons/light/87.png");
        i95 = QIcon(":icons/light/95.png");

    } else if (iconsMode == IconsMode::dark) {
        ipp = QIcon(":icons/dark/pp.png");
        i00 = QIcon(":icons/dark/00.png");
        i25 = QIcon(":icons/dark/25.png");
        i50 = QIcon(":icons/dark/50.png");
        i75 = QIcon(":icons/dark/75.png");
        i87 = QIcon(":icons/dark/87.png");
        i95 = QIcon(":icons/dark/95.png");
    } else {
        ipp = QIcon(":icons/logo.png");
        i00 = QIcon(":icons/logo.png");
        i25 = QIcon(":icons/logo.png");
        i50 = QIcon(":icons/logo.png");
        i75 = QIcon(":icons/logo.png");
        i87 = QIcon(":icons/logo.png");
        i95 = QIcon(":icons/logo.png");
    }

}


void TrayIcon::ShowDialog()
{
    dialog = new Dialog();
    connect(dialog, SIGNAL(closedialog()), this, SLOT(CloseDialog()));
    connect(dialog, SIGNAL(save(int, int, QString, QString, QString, bool, bool, bool, bool, bool, bool, QString, IconsMode)),
            this, SLOT(Save(int, int, QString, QString, QString, bool, bool, bool, bool, bool, bool, QString, IconsMode)));

    connect(dialog, SIGNAL(TimerStatusRequest()), this, SLOT(TimerStatusSend()));

    connect(this, SIGNAL(updateLabel(QString, double)), dialog, SLOT(UpdateLabel(QString, double)));

    ShowSettingAct->setEnabled(false);

    dialog->SetValues(pauseInterval, pauseContinuous, ImagesPath, ImagesPath_alt, imageAspectMode,
                      isLogging, isText, isClock, isMessage30sec, isPrettyFont, isStartupLink, Text, iconsMode);

#ifdef __linux__
    dialog->showNormal();
#else
    dialog->setWindowState((dialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    dialog->show();
#endif
}

void TrayIcon::TimerStatusSend()
{
    if (DialogTimer->isActive)
        emit updateLabel(DialogTimer->remains_str(false), DialogTimer->ratio());
    else
        emit updateLabel(DialogTimer->remains_str(false) +  " (" + tr("Pause") + ")", -DialogTimer->ratio());
}



void TrayIcon::DialogUpdateTime()
{
    qint64 remains =  DialogTimer->remains();;
    TimeRemains = DialogTimer->remains_str(false);
    double ratio = DialogTimer->ratio();


    emit updateLabel(TimeRemains, ratio);

    setToolTip(tr("Until break")+ ": "+ TimeRemains);

    if (isMessage30sec && remains < 30000 && !TrayMessageShowed) {
        if (isLogging) {
            QFile file("Logging.txt");
            if (file.open(QIODevice::Append)) {
                QTextStream out(&file);
                out << QString("%1, message. remains = %2; ")
                       .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"))
                       .arg(remains);
                file.close();
            }
        }

        showMessage(qApp->translate("App","Eyes' Thanks"), QString(tr("Until break") + " %1 " + tr("sec")).arg(qRound(remains / 1000.)));
        TrayMessageShowed = true;
    }


    setCurrentIcon(ratio);
}


void TrayIcon::RefreshmentStart()
{
    ShowView();
    DialogTimer->restart();
    TrayMessageShowed = false;
    CurrentIconRatio = -1;
    ViewTimer->start();
}


void TrayIcon::ShowView()
{
    view = new View();
    connect(ViewTimer, SIGNAL(finished()), view, SIGNAL(view_close()));
    //connect(view,      SIGNAL(view_close()),    view, SLOT(close()));

    QList<QString> pics_path;

    QStringList images = QDir(ImagesPath).entryList(QStringList() << "*.jpg" << "*.png");
    if (!images.isEmpty()) {
        QString pic_path = ImagesPath + "/" + images[qrand() % images.size()];
        pics_path.append(pic_path);
    }

    QStringList images_alt = QDir(ImagesPath_alt).entryList(QStringList() << "*.jpg" << "*.png");
    if (!images_alt.isEmpty()) {
        QString pic_path_alt = ImagesPath_alt + "/" + images_alt[qrand() % images_alt.size()];
        pics_path.append(pic_path_alt);
    }

    QString clock;
    if (isClock)  clock = QTime::currentTime().toString("hh:mm");
    else          clock = "";

    QString text;
    if (isText)    text = Text;
    else           text = "";

    text.replace("%interval", QString::number(pauseInterval / 1000 / 60) + " " + tr("min"));
    text.replace("%continuous", QString::number(pauseContinuous / 1000) + " " + tr("sec"));

    view->ShowRefreshment(pics_path, clock, text, isLogging, isPrettyFont, imageAspectMode);
}



void TrayIcon::ViewUpdateTime()
{
    if (view->isHidden()) {
        ViewTimer->stop();
        return;
    }

    view->UpdateValues(ViewTimer->remains_str(false), ViewTimer->ratio());
}

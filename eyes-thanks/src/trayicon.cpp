//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//
#include "aboutwindow.h"
#include "trayicon.h"

#include <QMenu>
#include <QApplication>

TrayIcon::TrayIcon(QWidget *parent): QSystemTrayIcon(parent),
    dialog(nullptr), TrayMessageShowed(false), TimeRemains(""), CurrentIconRatio(-1)
{
    qsrand(QTime::currentTime().msec());
    LangPath = "languages";
    Translator = new QTranslator();

    createLangActionGroup();

    createActions();
    createContextMenu();

    readSettings();

    for (auto f : QDir(":fonts").entryInfoList())
        QFontDatabase::addApplicationFont(f.absoluteFilePath());


    initIcons();

    setCurrentIcon(0.0);

    ViewTimer = new Timer(UPDATE_PERIOD_1, setting.pauseDuration);
    DialogTimer = new Timer(UPDATE_PERIOD_2, setting.pauseInterval, setting.isLogging);
    DialogTimer->start();

    connect(ViewTimer, SIGNAL(update()), this, SLOT(ViewUpdateTime()));
    connect(DialogTimer, SIGNAL(update()), this, SLOT(DialogUpdateTime()));
    connect(DialogTimer, SIGNAL(finished()), this, SLOT(RefreshmentStart()));
    connect(ViewTimer, SIGNAL(finished()), ViewTimer, SLOT(stop()));

    connect(DialogTimer, SIGNAL(finished()), DialogTimer, SLOT(stop()));


    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(Activated(QSystemTrayIcon::ActivationReason)));


    if (setting.running_counter == 1) ShowDialog();

#ifndef DEPLOY
    RefreshmentStart();
#endif
}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::createLangActionGroup()
{
    LangActGroup = new QActionGroup(this);
    LangActGroup->setExclusive(true); // Mutually exclusive checking
    connect(LangActGroup, SIGNAL(triggered(QAction *)), this, SLOT(LanguageChanged(QAction *)));

    QDir dir(LangPath);
    QStringList fileNames = dir.entryList(QStringList("lang_*.qm"));

    bool haveEnglish_qm = false;

    for (auto fileName : fileNames) { // "lang_ru.qm"
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

    if (!haveEnglish_qm) {
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

    TestAct        = new QAction(testIcon, "", this);
    connect(TestAct, SIGNAL(triggered()), this, SLOT(RefreshmentStart()));

    ShowSettingAct = new QAction(settingIcon, "", this);
    connect(ShowSettingAct, SIGNAL(triggered()), this, SLOT(ShowDialog()));

    PauseAct       = new QAction(pauseIcon, "", this);
    connect(PauseAct, SIGNAL(triggered()), this, SLOT(Pause()));

    UpdaterAct       = new UpdateAction(updateIcon, "", this);

    AboutAct       = new QAction(aboutIcon, "", this);
    connect(AboutAct, SIGNAL(triggered()), this, SLOT(About()));

    QuitAct        = new QAction(quitIcon, "", this);
    connect(QuitAct, SIGNAL(triggered()), this, SLOT(Quit()));
}

void TrayIcon::createContextMenu()
{
    QMenu *ContextMenu = new QMenu();
    ContextMenu->addAction(TestAct);
    ContextMenu->addAction(ShowSettingAct);
    ContextMenu->addAction(PauseAct);

    SubMenuLanguages = new QMenu("");
    SubMenuLanguages->addActions(LangActGroup->actions());
    ContextMenu->addMenu(SubMenuLanguages);

    ContextMenu->addAction(UpdaterAct);
    ContextMenu->addAction(AboutAct);
    ContextMenu->addAction(QuitAct);

    setContextMenu(ContextMenu);
}

void TrayIcon::readSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setIniCodec("utf8");

    settings.beginGroup("Settings");
    loadLanguage(settings.value("lang", QLocale::system().name().split('_').front()).toString());
    setting.running_counter         = settings.value("counter", 0).toInt();
    setting.running_counter++;
    setting.pauseInterval   = settings.value("interval", 60 * 60 * 1000).toInt();
    setting.pauseDuration = settings.value("duration", 60 * 1000).toInt();

    setting.imagesPath      = settings.value("background_path", "").toString();
    setting.imagesPathAlternative  = settings.value("background_path_alt", "").toString();

    setting.imageAspectMode = static_cast<ImageAspectMode>(settings.value("aspectMode", ImageAspectMode::Auto).toUInt());
    setting.iconsMode       = static_cast<IconsMode>(settings.value("iconsMode", IconsMode::light).toUInt());

    setting.isClock         = settings.value("clock_enabled", true).toBool();
    setting.isMessage30sec  = settings.value("message_enabled", true).toBool();
    setting.isLogging       = settings.value("logging_enabled", false).toBool();
    setting.isText          = settings.value("text_enabled", false).toBool();
    setting.isPrettyFont    = settings.value("prettyFont_enabled", true).toBool();

    if (setting.running_counter == 1) QFile::remove(
                QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk");

    setting.isStartupLink   = settings.value("startupLink_enabled", false).toBool();

    setting.text            = settings.value("text", qApp->translate("App","All work and no play\nmakes Jack a dull boy.")).toString();

    setting.isSpectrum       = settings.value("spectrum_enabled", true).toBool();
    setting.isTiling         = settings.value("tiling_enabled",   true).toBool();
    setting.isStripes        = settings.value("stripes_enabled",  false).toBool();
    setting.isCircle         = settings.value("circle_enabled",   true).toBool();
    setting.isCircles        = settings.value("circles_enabled",  true).toBool();
    setting.isNeo            = settings.value("neo_enabled",      true).toBool();

    settings.endGroup();
}

void TrayIcon::writeSettings()
{
    QSettings qsettings("settings.ini", QSettings::IniFormat);
    qsettings.setIniCodec("utf8");

    qsettings.beginGroup("Settings");
    qsettings.setValue("lang",                   LangCurrent);
    qsettings.setValue("counter",                setting.running_counter);
    qsettings.setValue("interval",               setting.pauseInterval);
    qsettings.setValue("duration",             setting.pauseDuration);

    qsettings.setValue("background_path",        setting.imagesPath);
    qsettings.setValue("background_path_alt",    setting.imagesPathAlternative);

    qsettings.setValue("iconsMode",              static_cast<uint>(setting.iconsMode));
    qsettings.setValue("aspectMode",             static_cast<uint>(setting.imageAspectMode));

    qsettings.setValue("clock_enabled",          setting.isClock);
    qsettings.setValue("message_enabled",        setting.isMessage30sec);
    qsettings.setValue("logging_enabled",        setting.isLogging);
    qsettings.setValue("prettyFont_enabled",     setting.isPrettyFont);
    qsettings.setValue("startupLink_enabled",    setting.isStartupLink);

    qsettings.setValue("text_enabled",           setting.isText);
    qsettings.setValue("text",                   setting.text);

    qsettings.setValue("spectrum_enabled",       setting.isSpectrum );
    qsettings.setValue("riling_enabled",         setting.isTiling   );
    qsettings.setValue("stripes_enabled",        setting.isStripes  );
    qsettings.setValue("circle_enabled",         setting.isCircle   );
    qsettings.setValue("circles_enabled",        setting.isCircles  );
    qsettings.setValue("neo_enabled",            setting.isNeo      );

    qsettings.endGroup();
}



void TrayIcon::CloseDialog()
{
    ShowSettingAct->setEnabled(true);
    dialog = nullptr;
}


void TrayIcon::createOrDeleteAppStartUpLink(bool create)
{

    QString startup_lnk =  QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                           QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk";

    if (create)
        QFile::link(QApplication::applicationFilePath(), startup_lnk);
    else
        QFile::remove(startup_lnk);
}

void TrayIcon::Save(const Setting &s)
{
    bool fullrestart = false;
    if (setting.pauseInterval != s.pauseInterval ||
            setting.pauseDuration != s.pauseDuration)
        fullrestart = true;

    // create and delete StartUp Icon, at save time.
    if (setting.isStartupLink != s.isStartupLink)
        createOrDeleteAppStartUpLink(s.isStartupLink);

    setting = s;

    writeSettings();
    initIcons();

    if (fullrestart) {
        CurrentIconRatio = -1;

        PauseAct->setText(tr("Pause"));
#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif
        DialogTimer->restart(UPDATE_PERIOD_2, setting.pauseInterval);
        ViewTimer->init(UPDATE_PERIOD_1, setting.pauseDuration);

        TrayMessageShowed = false;

        emit updateLabel(DialogTimer->remains_str(), DialogTimer->ratio());
    }

    setCurrentIconbyCurrentIconRatio();
}

void TrayIcon::Quit()
{
    writeSettings();
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
        emit updateLabel(DialogTimer->remains_str() +  " (" + tr("Pause") + ")", -DialogTimer->ratio());
    }
    else {
        PauseAct->setText(tr("Pause"));
#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/media-playback-pause.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif

        DialogTimer->unpause();

        setCurrentIconbyCurrentIconRatio();

        emit updateLabel(DialogTimer->remains_str(), DialogTimer->ratio());
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
void TrayIcon::LanguageChanged(QAction *action)
{
    if (0 != action) {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        writeSettings();
    }
}

// Called by ReadSetting
//-----------------------------------------------------------------
void TrayIcon::loadLanguage(const QString &rLanguage)
{
    if (LangCurrent != rLanguage && !rLanguage.isEmpty()) {

        // check, if there is *.qm file correspond to rLanguage
        bool thereIsQM = false;
        for (auto action : LangActGroup->actions())
            if (action->data().toString() == rLanguage)
                thereIsQM = true;


        if (thereIsQM)
            LangCurrent = rLanguage;
        else
            LangCurrent = "en";


        for (auto action : LangActGroup->actions())
            if (action->data().toString() == LangCurrent)
                action->setChecked(true);

        QLocale locale = QLocale(LangCurrent);
        QLocale::setDefault(locale);

        qApp->removeTranslator(Translator);

        bool load = Translator->load(QString("lang_%1.qm").arg(LangCurrent), LangPath);
        if (load)
            qApp->installTranslator(Translator);

        translate();
    }
}


//-----------------------------------------------------------------
void TrayIcon::translate()
{
    TestAct->setText(tr("Break Now"));
    ShowSettingAct->setText(tr("Setting"));
    PauseAct->setText(tr("Pause"));
    QuitAct->setText(tr("Quit"));
    AboutAct->setText(tr("About"));
    UpdaterAct->setText(tr("Check for Updates"));
    SubMenuLanguages->setTitle(tr("Languages"));

    if (dialog)
        dialog->Translate();
}


//-----------------------------------------------------------------
void TrayIcon::setCurrentIcon(qreal ratio)
{
    if (CurrentIconRatio == -1) {
        setIcon(i00);
        CurrentIconRatio = 0;
    }
    else if (ratio > 0.25 && CurrentIconRatio == 0) {
        setIcon(i25);
        CurrentIconRatio = 0.25;
    }
    else if (ratio > 0.50 && CurrentIconRatio == 0.25) {
        setIcon(i50);
        CurrentIconRatio = 0.5;
    }
    else if (ratio > 0.75 && CurrentIconRatio == 0.5) {
        setIcon(i75);
        CurrentIconRatio = 0.75;
    }
    else if (ratio > 0.87 && CurrentIconRatio == 0.75) {
        setIcon(i87);
        CurrentIconRatio = 0.87;
    }
    else if (ratio > 0.95 && CurrentIconRatio == 0.87) {
        setIcon(i95);
        CurrentIconRatio = 0.95;
    }
}

void TrayIcon::initIcons()
{
    if (setting.iconsMode == IconsMode::light) {
        ipp = QIcon(":icons/light/pp.png");
        i00 = QIcon(":icons/light/00.png");
        i25 = QIcon(":icons/light/25.png");
        i50 = QIcon(":icons/light/50.png");
        i75 = QIcon(":icons/light/75.png");
        i87 = QIcon(":icons/light/87.png");
        i95 = QIcon(":icons/light/95.png");

    }
    else if (setting.iconsMode == IconsMode::dark) {
        ipp = QIcon(":icons/dark/pp.png");
        i00 = QIcon(":icons/dark/00.png");
        i25 = QIcon(":icons/dark/25.png");
        i50 = QIcon(":icons/dark/50.png");
        i75 = QIcon(":icons/dark/75.png");
        i87 = QIcon(":icons/dark/87.png");
        i95 = QIcon(":icons/dark/95.png");
    }
    else {
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
    connect(dialog, SIGNAL(save(Setting)), this, SLOT(Save(Setting)));

    connect(dialog, SIGNAL(TimerStatusRequest()), this, SLOT(TimerStatusSend()));

    connect(this, SIGNAL(updateLabel(QString, qreal)), dialog, SLOT(UpdateLabel(QString, qreal)));

    ShowSettingAct->setEnabled(false);

    dialog->SetValues(setting);

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
        emit updateLabel(DialogTimer->remains_str(), DialogTimer->ratio());
    else
        emit updateLabel(DialogTimer->remains_str() +  " (" + tr("Pause") + ")", -DialogTimer->ratio());
}



void TrayIcon::DialogUpdateTime()
{
    qint64 remains =  DialogTimer->remains();;
    TimeRemains = DialogTimer->remains_str();
    qreal ratio = DialogTimer->ratio();


    emit updateLabel(TimeRemains, ratio);

    setToolTip(tr("Until break") + ": " + TimeRemains);

    if (setting.isMessage30sec && remains < 30000 && !TrayMessageShowed) {
        if (setting.isLogging) {
            QString message = QString("%1, 30sec'message. remains = %2;")
                              .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"))
                              .arg(remains);

            LogToFile("LoggingTimer.txt", message);
        }
        showMessage(qApp->translate("App", "Eyes' Thanks"), QString(tr("Until break") + " %1 " + tr("sec")).arg(qRound(remains / 1000.)));
        TrayMessageShowed = true;
    }

    setCurrentIcon(ratio);
}


void TrayIcon::RefreshmentStart()
{
    DialogTimer->stop();
    TrayMessageShowed = false;
    CurrentIconRatio = -1;
    DialogUpdateTime();

    ViewTimer->start();
    ShowView();
}


void TrayIcon::ShowView()
{

    QElapsedTimer timer; timer.start();
    qDebug().noquote() << QTime::currentTime().toString("ss.zzz") << "ShowView start";

    view = new View();
    connect(this, SIGNAL(show_refreshment(QList<QString>, QString, QString, Setting, Timer*)),
            view, SLOT(ShowRefreshment(QList<QString>, QString, QString, Setting, Timer*)));
    connect(ViewTimer, SIGNAL(finished()),   view,        SLOT(close())); // close() run view->closeEvent()
    //connect(view,      SIGNAL(view_close()), view,        SLOT(close()));
    connect(view,      SIGNAL(view_close()), DialogTimer, SLOT(start()));
    connect(view,      SIGNAL(view_close()), ViewTimer,   SLOT(stop()));

    qDebug().noquote() << QTime::currentTime().toString("ss.zzz") << "TrayIcon::ShowView(), view constuctor in" << timer.elapsed() << "ms." ;

    QList<QString> pics_path;

    QStringList images = QDir(setting.imagesPath).entryList(QStringList() << "*.jpg" << "*.png");
    if (!images.isEmpty()) {
        QString pic_path = setting.imagesPath + "/" + images[qrand() % images.size()];
        pics_path.append(pic_path);
    }

    QStringList images_alt = QDir(setting.imagesPathAlternative).entryList(QStringList() << "*.jpg" << "*.png");
    if (!images_alt.isEmpty()) {
        QString pic_path_alt = setting.imagesPathAlternative + "/" + images_alt[qrand() % images_alt.size()];
        pics_path.append(pic_path_alt);
    }

    QString clock;
    if (setting.isClock)   clock = QTime::currentTime().toString("hh:mm");
    else                   clock = "";

    QString text;
    if (setting.isText)    text = setting.text;
    else                   text = "";

    text.replace("%until", QString::number(setting.pauseInterval / 1000 / 60) + " " + tr("min"));
    text.replace("%duration", QString::number(setting.pauseDuration / 1000) + " " + tr("sec"));

    emit show_refreshment(pics_path, clock, text, setting, ViewTimer);

    qDebug().noquote() << QTime::currentTime().toString("ss.zzz") << "ShowView end in" << timer.elapsed() << "ms." ;
}



void TrayIcon::ViewUpdateTime()
{
    if (view)
        view->UpdateValues(ViewTimer->remains_str(), ViewTimer->ratio());
    else
        qDebug() << "trying to update nonexisting view";
}
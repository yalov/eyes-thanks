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

    QIcon testIcon    = QIcon(":icons/actions/video-display.png");
    QIcon settingIcon = QIcon(":icons/actions/configure.png");
    QIcon pauseIcon   = QIcon(":icons/actions/media-playback-pause.png");
    QIcon skipIcon    = QIcon(":icons/actions/media-skip-forward.png");
    QIcon updateIcon  = QIcon(":icons/actions/system-software-update");
    QIcon aboutIcon   = QIcon(":icons/actions/help-about.png");
    QIcon quitIcon    = QIcon(":icons/actions/application-exit.png");

#else
    QIcon testIcon    = QIcon::fromTheme("video-display"); // preferences-desktop-display
    QIcon settingIcon = QIcon::fromTheme("document-properties");//QIcon::fromTheme( "configure" );
    QIcon pauseIcon   = QIcon::fromTheme("media-playback-pause");//QWidget().style()->standardIcon(QStyle::SP_MediaPause);
    QIcon skipIcon    = QIcon::fromTheme("media-skip-forward");
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

    SkipAct       = new QAction(skipIcon, "", this);
    connect(SkipAct, SIGNAL(triggered()), this, SLOT(Skip()));


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
    ContextMenu->addAction(SkipAct);

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
    setting.pauseInterval   = settings.value("interval", 59 * 60 * 1000).toInt();
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

    bool isStartupLinkdefault = false;
    if (setting.running_counter == 1) {
        QString startup_lnk =  QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                               QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk";

        if (QFile::exists(startup_lnk)) {
            isStartupLinkdefault = true;
            QFile::remove(startup_lnk);
            QFile::link(QApplication::applicationFilePath(), startup_lnk);
        }
        else{
            isStartupLinkdefault = false;
        }
    }

    setting.isStartupLink   = settings.value("startupLink_enabled", isStartupLinkdefault).toBool();

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
        PauseAct->setIcon(QIcon(":icons/actions/media-playback-pause.png"));
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
        PauseAct->setIcon(QIcon(":icons/actions/media-playback-start.png"));
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
        PauseAct->setIcon(QIcon(":icons/actions/media-playback-pause.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif

        DialogTimer->unpause();

        setCurrentIconbyCurrentIconRatio();

        emit updateLabel(DialogTimer->remains_str(), DialogTimer->ratio());
    }
}

void TrayIcon::Skip()
{
    DialogTimer->restart();
    CurrentIconRatio = -1;
    setCurrentIconbyCurrentIconRatio();
    emit updateLabel(DialogTimer->remains_str(), DialogTimer->ratio());
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
    SkipAct->setText(tr("Skip"));
    QuitAct->setText(tr("Quit"));
    AboutAct->setText(tr("About"));
    UpdaterAct->setText(tr("Check for Updates"));
    SubMenuLanguages->setTitle(tr("Languages"));

    if (dialog)
        dialog->Translate();
}


//-----------------------------------------------------------------

void TrayIcon::setPauseIcon()
{
    setIcon(ipp);
}

void TrayIcon::setCurrentIconbyCurrentIconRatio()
{
    if      (CurrentIconRatio == 2/16.0) setIcon(i02);
    else if (CurrentIconRatio == 4/16.0) setIcon(i04);
    else if (CurrentIconRatio == 6/16.0) setIcon(i06);
    else if (CurrentIconRatio == 8/16.0) setIcon(i08);
    else if (CurrentIconRatio == 10/16.0) setIcon(i10);
    else if (CurrentIconRatio == 12/16.0) setIcon(i12);
    else if (CurrentIconRatio == 14/16.0) setIcon(i14);
    else if (CurrentIconRatio == 15/16.0) setIcon(i15);
    else                               setIcon(i00);  //if CurrentIconRatio == 0 or -1
}


void TrayIcon::setCurrentIcon(qreal ratio)
{
    if (CurrentIconRatio == -1) {
        setIcon(i00);
        CurrentIconRatio = 0;
    }
    else if (ratio > 1/16.0 && CurrentIconRatio == 0) {
        setIcon(i02);
        CurrentIconRatio = 2/16.0;
    }
    else if (ratio > 3/16.0 && CurrentIconRatio == 2/16.0) {
        setIcon(i04);
        CurrentIconRatio = 4/16.0;
    }
    else if (ratio > 5/16.0 && CurrentIconRatio == 4/16.0) {
        setIcon(i06);
        CurrentIconRatio = 6/16.0;
    }
    else if (ratio > 7/16.0 && CurrentIconRatio == 6/16.0) {
        setIcon(i08);
        CurrentIconRatio = 8/16.0;
    }
    else if (ratio > 9/16.0 && CurrentIconRatio == 8/16.0) {
        setIcon(i10);
        CurrentIconRatio = 10/16.0;
    }
    else if (ratio > 11/16.0 && CurrentIconRatio == 10/16.0) {
        setIcon(i12);
        CurrentIconRatio = 12/16.0;
    }
    else if (ratio > 13/16.0 && CurrentIconRatio == 12/16.0) {
        setIcon(i14);
        CurrentIconRatio = 14/16.0;
    }
    else if (ratio > 14.5/16.0 && CurrentIconRatio == 14/16.0) {
        setIcon(i15);
        CurrentIconRatio = 15/16.0;
    }
}

void TrayIcon::initIcons()
{
    if (setting.iconsMode == IconsMode::light) {
        ipp = QIcon(":icons/light/pp.png");
        i00 = QIcon(":icons/light/00.png");
        i02 = QIcon(":icons/light/12.png");
        i04 = QIcon(":icons/light/25.png");
        i06 = QIcon(":icons/light/37.png");
        i08 = QIcon(":icons/light/50.png");
        i10 = QIcon(":icons/light/62.png");
        i12 = QIcon(":icons/light/75.png");
        i14 = QIcon(":icons/light/87.png");
        i15 = QIcon(":icons/light/95.png");

    }
    else if (setting.iconsMode == IconsMode::dark) {
        ipp = QIcon(":icons/dark/pp.png");
        i00 = QIcon(":icons/dark/00.png");
        i02 = QIcon(":icons/dark/12.png");
        i04 = QIcon(":icons/dark/25.png");
        i06 = QIcon(":icons/dark/37.png");
        i08 = QIcon(":icons/dark/50.png");
        i10 = QIcon(":icons/dark/62.png");
        i12 = QIcon(":icons/dark/75.png");
        i14 = QIcon(":icons/dark/87.png");
        i15 = QIcon(":icons/dark/95.png");
    }
    else { // setting.iconsMode == IconsMode::white
        ipp = QIcon(":icons/white/pp.png");
        i00 = QIcon(":icons/white/00.png");
        i02 = QIcon(":icons/white/12.png");
        i04 = QIcon(":icons/white/25.png");
        i06 = QIcon(":icons/white/37.png");
        i08 = QIcon(":icons/white/50.png");
        i10 = QIcon(":icons/white/62.png");
        i12 = QIcon(":icons/white/75.png");
        i14 = QIcon(":icons/white/87.png");
        i15 = QIcon(":icons/white/95.png");
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

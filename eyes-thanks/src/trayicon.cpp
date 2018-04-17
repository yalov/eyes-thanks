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

#ifdef false
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

        auto locale = fileName.split('.').first().split('_').last();  // en, ru

        if (locale == "en") haveEnglish_qm = true;

        QString lang = translator.translate("App", "Language_name");

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
    connect(SkipAct, SIGNAL(triggered()), this, SLOT(Restart()));


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

    bool isStartupLinkdefault = CheckStartupLink();

    setting.isStartupLink   = settings.value("startupLink_enabled", isStartupLinkdefault).toBool();

    setting.text            = settings.value("text", qApp->translate("App", "Proverbs.").split("\n\n")[0]).toString();

    setting.isSpectrum       = settings.value("spectrum_enabled", true).toBool();
    setting.isTiling         = settings.value("tiling_enabled",   true).toBool();
    setting.isStripes        = settings.value("stripes_enabled",  false).toBool();
    setting.isCircle         = settings.value("circle_enabled",   true).toBool();
    setting.isCircles        = settings.value("circles_enabled",  true).toBool();
    setting.isNeo            = settings.value("neo_enabled",      true).toBool();

    settings.endGroup();


    if (setting.running_counter == 1) writeSettings();
}

bool TrayIcon::CheckStartupLink(){
    bool isStartupLinkdefault = false;
    if (setting.running_counter == 1) {

        QString startup_lnk =  QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                QDir::separator() + "Startup" + QDir::separator() + "Eyes' Thanks.lnk";

        bool is_link = QFile::exists(startup_lnk);
        QString link_trg = QFile(startup_lnk).symLinkTarget();

        if (is_link && link_trg == QApplication::applicationFilePath())
            return true;

        QMessageBox * mb = new QMessageBox();
        mb->setWindowTitle(qApp->translate("App", "Eyes' Thanks"));
        mb->setWindowIcon(QIcon(":icons/logo.png"));
        mb->setIcon(QMessageBox::Question);
        //mb->setIconPixmap(QPixmap(":icons/logo.png").scaledToWidth(64, Qt::SmoothTransformation));

        QPushButton * Accept, * Reject;

        if (is_link)
        {
            mb->setText(tr("Shortcut is founded in the Startup Folder.").arg("<br><nobr>" + link_trg+"</nobr>"));
            mb->setInformativeText("<nobr>" + tr("Replace?") + "</nobr>" );

            Accept = mb->addButton(tr("Accept"), QMessageBox::AcceptRole);
            Reject = mb->addButton(tr("Leave"), QMessageBox::RejectRole);
        }
        else
        {
            mb->setText(tr("It is the first launch of Eyes' Thanks."));
            mb->setInformativeText(tr("Do you want to run the app on Windows startup?"));

            Accept = mb->addButton(tr("Yes"), QMessageBox::YesRole);
            Reject = mb->addButton(tr("No"), QMessageBox::NoRole);
        }

        mb->setDefaultButton(Accept);
        mb->setEscapeButton(Reject);

        mb->exec();

        if (mb->clickedButton() == Accept)
        {
            QFile::remove(startup_lnk);
            QFile::link(QApplication::applicationFilePath(), startup_lnk);
            isStartupLinkdefault = true;
        }
        else
        {
            isStartupLinkdefault = false;
        }
    }
    return isStartupLinkdefault;
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
    qsettings.setValue("tiling_enabled",         setting.isTiling   );
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

    if (create){
        QFile::remove(startup_lnk);
        QFile::link(QApplication::applicationFilePath(), startup_lnk);
    }
    else
        QFile::remove(startup_lnk);
}

void TrayIcon::Save(const Setting &s)
{
    bool need_fullrestart = false;
    if (setting.pauseInterval != s.pauseInterval ||
            setting.pauseDuration != s.pauseDuration)
        need_fullrestart = true;

    // create and delete StartUp Icon, at save time.
    if (setting.isStartupLink != s.isStartupLink)
        createOrDeleteAppStartUpLink(s.isStartupLink);

    setting = s;

    writeSettings();
    initIcons();

    if (need_fullrestart)
        Restart();

    setCurrentIconbyCurrentIconRatio();
}

void TrayIcon::Quit()
{
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
        setPauseAct(false);
        setPauseIcon();

        DialogTimer->pause();

        auto pause_str = QString(" (%1)").arg(tr("Pause"));
        setToolTip(qApp->translate("App", "Until break") + ": " + TimeRemains + pause_str);
        emit updateLabelPause(DialogTimer->remains_str() + pause_str, -DialogTimer->ratio());
    }
    else {
        setPauseAct(true);
        DialogTimer->unpause();

        setCurrentIconbyCurrentIconRatio();

        emit updateLabelPause(DialogTimer->remains_str(), DialogTimer->ratio());
    }
}


void TrayIcon::setPauseAct(bool setpause)
{
    if (!setpause) {
        PauseAct->setText(tr("Unpause"));

#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/actions/media-playback-start.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-start"));
#endif
    }
    else {
        PauseAct->setText(tr("Pause"));
#ifdef _WIN32
        PauseAct->setIcon(QIcon(":icons/actions/media-playback-pause.png"));
#else
        PauseAct->setIcon(QIcon::fromTheme("media-playback-pause"));
#endif
    }
}

void TrayIcon::restartGui(){

    CurrentIconRatio = -1;
    setCurrentIconbyCurrentIconRatio();
    TrayMessageShowed = false;
    setPauseAct(true);
    DialogUpdateTime();
}

void TrayIcon::Restart()
{
    DialogTimer->restart(UPDATE_PERIOD_2, setting.pauseInterval);
    ViewTimer->init(UPDATE_PERIOD_1, setting.pauseDuration);
    restartGui();
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

    connect(dialog, SIGNAL(TimerStatusRequest()), this, SLOT(DialogUpdateTime()));

    connect(this, SIGNAL(updateLabel(QString, qreal)), dialog, SLOT(UpdateLabel(QString, qreal)));
    connect(this, SIGNAL(updateLabelPause(QString, qreal)), dialog, SLOT(UpdateLabelPause(QString, qreal)));

    ShowSettingAct->setEnabled(false);

    dialog->SetValues(setting);

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
    TimeRemains = DialogTimer->remains_str();
    qreal ratio = DialogTimer->ratio();

    if (ViewTimer->isActive) {
        if (DialogTimer->isActive)
            emit updateLabel(TimeRemains, ratio);
        else
            emit updateLabel(TimeRemains +  " (" + tr("Pause") + ")", -ratio);
    }
    else{
            emit updateLabelPause(TimeRemains, ratio);
    }

    setToolTip(qApp->translate("App", "Until break") + ": " + TimeRemains);

    if (setting.isMessage30sec && remains < 30000 && !TrayMessageShowed) {
        if (setting.isLogging) {
            QString message = QString("%1, 30sec'message. remains = %2;")
                              .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"))
                              .arg(remains);

            LogToFile("LoggingTimer.txt", message);
        }
        showMessage(qApp->translate("App", "Eyes' Thanks"), QString(qApp->translate("App", "Until break") + " %1 " + qApp->translate("App", "sec")).arg(qRound(remains / 1000.)));
        TrayMessageShowed = true;
    }

    setCurrentIcon(ratio);
}


void TrayIcon::RefreshmentStart()
{
    DialogTimer->stop();
    restartGui();

    ViewTimer->start();
    ShowView();
}


void TrayIcon::ShowView()
{

    QElapsedTimer timer; timer.start();
    qDebug().noquote()  << "TrayIcon::ShowView(), start";

    view = new View();
    connect(this, SIGNAL(show_refreshment(QString, QString, Setting, Timer*)),
            view, SLOT(ShowRefreshment(QString, QString, Setting, Timer*)));
    connect(ViewTimer, SIGNAL(finished()),   view,        SLOT(close())); // close() run view->closeEvent()
    //connect(view,      SIGNAL(view_close()), view,        SLOT(close()));
    connect(view,      SIGNAL(view_close()), DialogTimer, SLOT(start()));
    connect(view,      SIGNAL(view_close()), ViewTimer,   SLOT(stop()));

    qDebug().noquote()  << "TrayIcon::ShowView(), view constuctor in" << timer.elapsed() << "ms." ;





    QString clock = "";
    QString text = "";
    if (setting.isClock)   clock = QTime::currentTime().toString("hh:mm"); 
    if (setting.isText)
    {
        text = setting.text;
        text.replace("%until", QString::number(setting.pauseInterval / 1000 / 60) + " " + qApp->translate("App", "min"));
        text.replace("%duration", QString::number(setting.pauseDuration / 1000) + " " + qApp->translate("App", "sec"));
    }

    emit show_refreshment(clock, text, setting, ViewTimer);

    qDebug().noquote()  << "TrayIcon::ShowView end in" << timer.elapsed() << "ms." ;
}



void TrayIcon::ViewUpdateTime()
{
    if (view)
        view->UpdateValues(ViewTimer->remains_str(), ViewTimer->ratio());
    else
        qDebug() << "trying to update nonexisting view";
}

//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//
#include "aboutwindow.h"
#include "trayicon.h"

#include <QMenu>
#include <QApplication>
#include <QtGlobal>

TrayIcon::TrayIcon(QWidget *parent): QSystemTrayIcon(parent)
{
    LangPath = "languages";
    Translator = new QTranslator();

    createLangActionGroup();

    createActions();
    createContextMenu();

    readSettings();

    for (const auto& f : QDir(":fonts").entryInfoList())
        QFontDatabase::addApplicationFont(f.absoluteFilePath());


    initIcons();

    setCurrentIcon(0.0);

    ViewTimer = new Timer(UPDATE_PERIOD_1, setting.pauseDuration);
    DialogTimer = new Timer(UPDATE_PERIOD_2, setting.pauseInterval, setting.isLogging);
    DialogTimer->start();

    connect(ViewTimer, &Timer::update, this, &TrayIcon::ViewUpdateTime);
    connect(DialogTimer, &Timer::update, this, &TrayIcon::DialogUpdateTime);
    connect(DialogTimer, &Timer::finished, this, &TrayIcon::RefreshmentStart);
    connect(ViewTimer, &Timer::finished, ViewTimer, &Timer::stop);

    connect(DialogTimer, &Timer::finished, DialogTimer, &Timer::stop);

    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::Activated);


    if (setting.running_counter == 1) ShowDialog();

#ifdef false
    RefreshmentStart();
#endif
}


//TrayIcon::~TrayIcon() = default;


void TrayIcon::createLangActionGroup()
{
    LangActGroup = new QActionGroup(this);
    LangActGroup->setExclusive(true); // Mutually exclusive checking
    connect(LangActGroup, &QActionGroup::triggered, this, &TrayIcon::LanguageChanged);

    QDir dir(LangPath);
    QStringList fileNames = dir.entryList(QStringList("lang_*.qm"));

    bool haveEnglish_qm = false;

    for (const auto& fileName : fileNames) { // "lang_ru.qm"
        QTranslator translator;
        bool success = translator.load(fileName, LangPath);

        if (success)
        {
            auto locale = fileName.split('.').first().split('_').last();  // en, ru

            if (locale == "en") haveEnglish_qm = true;

            QString lang = translator.translate("App", "Language_name");

            QAction *action = new QAction(lang, this);
            action->setCheckable(true);
            action->setData(locale);

            LangActGroup->addAction(action);
        }
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
    connect(TestAct, &QAction::triggered, this, &TrayIcon::RefreshmentStart);

    ShowSettingAct = new QAction(settingIcon, "", this);
    connect(ShowSettingAct, &QAction::triggered, this, &TrayIcon::ShowDialog);

    PauseAct       = new QAction(pauseIcon, "", this);
    connect(PauseAct, &QAction::triggered, this, &TrayIcon::Pause);

    SkipAct       = new QAction(skipIcon, "", this);
    connect(SkipAct, &QAction::triggered, this, &TrayIcon::Restart);


    UpdaterAct       = new UpdateAction(updateIcon, "", this);

    AboutAct       = new QAction(aboutIcon, "", this);
    connect(AboutAct, &QAction::triggered, this, &TrayIcon::About);

    QuitAct        = new QAction(quitIcon, "", this);
    connect(QuitAct, &QAction::triggered, this, &TrayIcon::Quit);
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("utf8");
#endif
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
    setting.isSkipWhenFullScreen = settings.value("skipWhenFullScreen_enabled", true).toBool();
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
            mb->setText(tr("Shortcut is founded in the Startup Folder to %1").arg("<br><nobr>" + link_trg+"</nobr>"));
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qsettings.setIniCodec("utf8");
#endif
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
    qsettings.setValue("skipWhenFullScreen_enabled", setting.isSkipWhenFullScreen);
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
    if (DialogTimer->isActive()) {
        setPauseAct(false);
        setPauseIcon();

        DialogTimer->pause();

        auto pause_str = QString(" (%1)").arg(tr("Pause"));
        setToolTip(qApp->translate("App", "Until break") + ": " + TimeRemains + pause_str);
        emit updateLabelPause(DialogTimer->remains_str() + pause_str);
    }
    else {
        setPauseAct(true);
        DialogTimer->unpause();

        setCurrentIconbyCurrentIconRatio();

        emit updateLabelPause(DialogTimer->remains_str());
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

    CurrentIconIndex = -1;
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
    if (action != nullptr) {
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
    if      (CurrentIconIndex == 2 ) setIcon(i02);
    else if (CurrentIconIndex == 4 ) setIcon(i04);
    else if (CurrentIconIndex == 6 ) setIcon(i06);
    else if (CurrentIconIndex == 8 ) setIcon(i08);
    else if (CurrentIconIndex == 10) setIcon(i10);
    else if (CurrentIconIndex == 12) setIcon(i12);
    else if (CurrentIconIndex == 14) setIcon(i14);
    else if (CurrentIconIndex == 15) setIcon(i15);
    else                             setIcon(i00);  //if CurrentIconRatio == 0 or -1
}

void TrayIcon::setCurrentIcon(qreal ratio)
{
    if      (CurrentIconIndex == -1)                    { setIcon(i00); CurrentIconIndex =  0; }
    else if (CurrentIconIndex ==  0 && ratio >  1/16.0) { setIcon(i02); CurrentIconIndex =  2; }
    else if (CurrentIconIndex ==  2 && ratio >  3/16.0) { setIcon(i04); CurrentIconIndex =  4; }
    else if (CurrentIconIndex ==  4 && ratio >  5/16.0) { setIcon(i06); CurrentIconIndex =  6; }
    else if (CurrentIconIndex ==  6 && ratio >  7/16.0) { setIcon(i08); CurrentIconIndex =  8; }
    else if (CurrentIconIndex ==  8 && ratio >  9/16.0) { setIcon(i10); CurrentIconIndex = 10; }
    else if (CurrentIconIndex == 10 && ratio > 11/16.0) { setIcon(i12); CurrentIconIndex = 12; }
    else if (CurrentIconIndex == 12 && ratio > 13/16.0) { setIcon(i14); CurrentIconIndex = 14; }
    else if (CurrentIconIndex == 14 && ratio>14.5/16.0) { setIcon(i15); CurrentIconIndex = 15; }
}

void TrayIcon::initIcons()
{
    const QString mode = setting.iconsMode == IconsMode::light ? "light" :
                         setting.iconsMode == IconsMode::dark  ? "dark"  : "white";
    auto icon = [&mode](const QString &name) {
        return QIcon(":icons/" + mode + "/" + name + ".png");
    };
    ipp = icon("pp");
    i00 = icon("00");
    i02 = icon("12");
    i04 = icon("25");
    i06 = icon("37");
    i08 = icon("50");
    i10 = icon("62");
    i12 = icon("75");
    i14 = icon("87");
    i15 = icon("95");
}

void TrayIcon::ShowDialog()
{
    dialog = new Dialog();
    connect(dialog, &Dialog::closedialog,        this,   &TrayIcon::CloseDialog);
    connect(dialog, &Dialog::save,               this,   &TrayIcon::Save);
    connect(dialog, &Dialog::TimerStatusRequest, this,   &TrayIcon::DialogUpdateTime);
    connect(this,   &TrayIcon::updateLabel,      dialog, &Dialog::UpdateLabel);
    connect(this,   &TrayIcon::updateLabelPause, dialog, &Dialog::UpdateLabelPause);

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
    qint64 remains =  DialogTimer->remains();
    TimeRemains = DialogTimer->remains_str();
    qreal ratio = DialogTimer->ratio();

    if (ViewTimer->isActive()) {
        if (DialogTimer->isActive())
            emit updateLabel(TimeRemains);
        else
            emit updateLabel(TimeRemains +  " (" + tr("Pause") + ")");
    }
    else{
            emit updateLabelPause(TimeRemains);
    }

    setToolTip(qApp->translate("App", "Until break") + ": " + TimeRemains);

    if (setting.isMessage30sec && remains < 30000 && !TrayMessageShowed) {
        if (setting.isLogging) {
            QString message = QString("%1, 30sec'message. remains = %2;")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
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
    connect(this,      &TrayIcon::show_refreshment, view,        &View::ShowRefreshment);
    connect(ViewTimer, &Timer::finished,             view,        &QWidget::close);
    connect(view,      &View::view_close,            DialogTimer, &Timer::start);
    connect(view,      &View::view_close,            ViewTimer,   &Timer::stop);

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

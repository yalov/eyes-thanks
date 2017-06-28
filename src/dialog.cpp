//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "dialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QApplication>

Dialog::Dialog():QDialog()
{
    setWindowIcon(QIcon(":icons/logo.png"));

#ifdef _WIN32
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

#endif

    InitWidgets();
    Translate();

}

void Dialog::InitWidgets()
{
    QVBoxLayout *TopLayout = new QVBoxLayout();
    setLayout(TopLayout);

    Label_Path = new QLabel();
    LineEdit_Path = new QLineEdit();
    QPushButton *ButtonPath = new QPushButton("...");
    ButtonPath->setMaximumWidth(25);

    label_Path_alt = new QLabel();
    LineEdit_Path_alt = new QLineEdit();
    QPushButton *ButtonPath_alt = new QPushButton("...");
    ButtonPath_alt->setMaximumWidth(25);

    Spinbox_RefreshmentInterval   = new QSpinBox();
    Spinbox_RefreshmentContinuous = new QSpinBox();
    Spinbox_RefreshmentInterval->setRange(1, 24 * 60);
    Spinbox_RefreshmentContinuous->setRange(0, 60 * 60);

    CheckBox_Clock       = new QCheckBox();
    CheckBox_Message     = new QCheckBox();
    CheckBox_Logging     = new QCheckBox();
    CheckBox_PrettyFont  = new QCheckBox();
    CheckBox_Text        = new QCheckBox();
    CheckBox_StartupLink = new QCheckBox();

    TextEdit_Text = new QPlainTextEdit();
    TextEdit_Text->setFixedHeight(70);

    ButtonGenerateText = new QPushButton();
    Combobox_imageAspectMode = new QComboBox();
    Combobox_iconsMode = new QComboBox();

    Label_ImageAspectMode = new QLabel();
    Label_UntilBreak      = new QLabel();
    Label_BreakContinuous = new QLabel();
    Label_TrayIconStyle = new QLabel();

    buttonSave = new QPushButton();
    buttonSave->setIcon(QIcon(":/icons/save.png"));
    buttonSave->setEnabled(false);
    buttonMinimizeToSystemTray = new QPushButton();
    //buttonMinimizeToSystemTray->setIcon(QIcon(":/icons/save.png"));
    buttonSave->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    buttonMinimizeToSystemTray->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


    //-----------------------------
    GroupBox_BreakSetting = new QGroupBox();
    QVBoxLayout *layoutV_BreakSetting = new QVBoxLayout();
    GroupBox_BreakSetting->setLayout(layoutV_BreakSetting);

    QHBoxLayout *layout_Path = new QHBoxLayout();
    layout_Path->addWidget(LineEdit_Path);
    layout_Path->addWidget(ButtonPath);

    QHBoxLayout *layout_Path_alt = new QHBoxLayout();
    layout_Path_alt->addWidget(LineEdit_Path_alt);
    layout_Path_alt->addWidget(ButtonPath_alt);

    layoutV_BreakSetting->addWidget(Label_Path);
    layoutV_BreakSetting->addLayout(layout_Path);
    layoutV_BreakSetting->addWidget(label_Path_alt);
    layoutV_BreakSetting->addLayout(layout_Path_alt);

    QGridLayout *layoutGrid_BreakSetting = new QGridLayout();
    layoutGrid_BreakSetting->addWidget(Label_ImageAspectMode,         0, 0);
    layoutGrid_BreakSetting->addWidget(Combobox_imageAspectMode,      0, 1);
    layoutGrid_BreakSetting->addWidget(Label_UntilBreak,              1, 0);
    layoutGrid_BreakSetting->addWidget(Spinbox_RefreshmentInterval,   1, 1);
    layoutGrid_BreakSetting->addWidget(Label_BreakContinuous,         2, 0);
    layoutGrid_BreakSetting->addWidget(Spinbox_RefreshmentContinuous, 2, 1);
    layoutGrid_BreakSetting->addWidget(CheckBox_PrettyFont,  7, 0, 1, 2);
    layoutGrid_BreakSetting->addWidget(CheckBox_Clock,       8, 0, 1, 2);
    layoutGrid_BreakSetting->addWidget(CheckBox_Text,        9, 0);
    layoutGrid_BreakSetting->addWidget(ButtonGenerateText,   9, 1);
    layoutGrid_BreakSetting->addWidget(TextEdit_Text,        10, 0, 1, 2);

    layoutGrid_BreakSetting->setColumnStretch(0, 2);
    layoutGrid_BreakSetting->setColumnStretch(1, 1);

    layoutV_BreakSetting->addLayout(layoutGrid_BreakSetting);

    TopLayout->addWidget(GroupBox_BreakSetting);

    TopLayout->addSpacing(30);
    TopLayout->addStretch();

    //----------

    GroupBox_SystemSetting = new QGroupBox();
    QGridLayout *layoutGrid_SystemSetting = new QGridLayout();
    GroupBox_SystemSetting->setLayout(layoutGrid_SystemSetting);

    layoutGrid_SystemSetting->addWidget(Label_TrayIconStyle,  0, 0);
    layoutGrid_SystemSetting->addWidget(Combobox_iconsMode,   0, 1);
    layoutGrid_SystemSetting->addWidget(CheckBox_StartupLink, 2, 0, 1, 2);
    layoutGrid_SystemSetting->addWidget(CheckBox_Message,     3, 0, 1, 2);
    layoutGrid_SystemSetting->addWidget(CheckBox_Logging,     4, 0, 1, 2);

    layoutGrid_SystemSetting->setColumnStretch(0, 2);
    layoutGrid_SystemSetting->setColumnStretch(1, 1);

    TopLayout->addWidget(GroupBox_SystemSetting);

    TopLayout->addStretch();
    TopLayout->addSpacing(15);

    //----------

    QHBoxLayout *layout_Label_timer = new QHBoxLayout();

    Label_Timer_Prefix = new QLabel();

    Label_Timer = new QLabel();
    QFont font;
    font.setPointSize(16);
    Label_Timer->setFont(font);
    //Label_Timer->setAlignment(Qt::AlignCenter);
    //Label_Timer_Prefix->setAlignment(Qt::AlignCenter);
    Label_Timer->setAlignment(Qt::AlignLeft);
    //Label_Timer_Prefix->setAlignment(Qt::AlignRight);

    Label_Timer_Prefix->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    Label_Timer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    layout_Label_timer->addWidget(Label_Timer_Prefix);
    layout_Label_timer->addWidget(Label_Timer);
    layout_Label_timer->setStretch(0, 2);
    layout_Label_timer->setStretch(1, 1);

    TopLayout->addLayout(layout_Label_timer);
    TopLayout->addStretch();
    TopLayout->addSpacing(15);

    //-----
    QHBoxLayout *layout_buttons = new QHBoxLayout();

    layout_buttons->addWidget(buttonSave);
    layout_buttons->addWidget(buttonMinimizeToSystemTray);

    TopLayout->addLayout(layout_buttons);



#ifdef Q_OS_WIN32

    TaskbarButton = new QWinTaskbarButton(this);
    //TaskbarButton->setOverlayIcon(QIcon(":icons/50.png"));

    TaskbarProgress = TaskbarButton->progress();
    TaskbarProgress->setVisible(true);

#endif

    connect(ButtonPath, SIGNAL(clicked()), this, SLOT(ButtonPath_clicked()));
    connect(ButtonPath_alt, SIGNAL(clicked()), this, SLOT(ButtonPath_alt_clicked()));
    connect(ButtonGenerateText, SIGNAL(clicked()), this, SLOT(ButtonGenerateText_clicked()));

    connect(CheckBox_Text, SIGNAL(toggled(bool)), TextEdit_Text, SLOT(setEnabled(bool)));
    connect(CheckBox_Text, SIGNAL(toggled(bool)), ButtonGenerateText, SLOT(setEnabled(bool)));

    connect(buttonSave, SIGNAL(clicked()), this, SLOT(SaveValues()));
    connect(buttonMinimizeToSystemTray, SIGNAL(clicked()), this, SLOT(close()));




}

void Dialog::InitConnectWidgetsChanged()
{
    // after ::SetValues
    connect(buttonSave, SIGNAL(clicked(bool)), buttonSave, SLOT(setEnabled(bool)));

    connect(LineEdit_Path,     SIGNAL(textChanged(QString)), this, SLOT(SaveButton_status()));
    connect(LineEdit_Path_alt, SIGNAL(textChanged(QString)), this, SLOT(SaveButton_status()));
    connect(TextEdit_Text,     SIGNAL(textChanged()), this, SLOT(SaveButton_status()));

    connect(Combobox_imageAspectMode, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveButton_status()));
    connect(Combobox_iconsMode,       SIGNAL(currentIndexChanged(int)), this, SLOT(SaveButton_status()));

    connect(Spinbox_RefreshmentInterval,   SIGNAL(valueChanged(int)), this, SLOT(SaveButton_status()));
    connect(Spinbox_RefreshmentContinuous, SIGNAL(valueChanged(int)), this, SLOT(SaveButton_status()));

    connect(CheckBox_Clock,       SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Message,     SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Logging,     SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_PrettyFont,  SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Text,        SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_StartupLink, SIGNAL(clicked()), this, SLOT(SaveButton_status()));
}

void Dialog::SaveButton_status()
{
    Setting s = {
        setting.counter,
        Spinbox_RefreshmentInterval->value() * 60 * 1000,
        Spinbox_RefreshmentContinuous->value() * 1000,
        LineEdit_Path->text(),
        LineEdit_Path_alt->text(),
        static_cast<ImageAspectMode>(Combobox_imageAspectMode->currentIndex()),
        static_cast<IconsMode>(Combobox_iconsMode->currentIndex()),
        CheckBox_Logging->isChecked(),
        CheckBox_Text->isChecked(),
        CheckBox_Clock->isChecked(),
        CheckBox_Message->isChecked(),
        CheckBox_PrettyFont->isChecked(),
        CheckBox_StartupLink->isChecked(),
        TextEdit_Text->toPlainText()
    };


    if (setting != s)
        buttonSave->setEnabled(true);
    else
        buttonSave->setEnabled(false);

//    qDebug() << "SaveButton_status";

}


void Dialog::Translate()
{
    setWindowTitle(qApp->translate("App", "Eyes' Thanks"));

    Label_Path->setText(tr("Pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    Label_Path->setToolTip(tr("Leave both folder paths empty, if you want nice gradient instead of pictures."));

    label_Path_alt->setText(tr("Alternative pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    label_Path_alt->setToolTip(tr("Use, if you have two different-ratio sets of monitors.\n\n"
                                  "Ex. if sometimes you disconnect your FHD notebook from your FHD monitor,\n"
                                  "put 2FHD (3860×1080) pictures to “Pictures folder” and\n"
                                  "FHD (1920×1080) pictures to “Alternative pictures folder”).\n"
                                  "So, connect laptop to monitor — app use 2FHD folder, laptop by itself — app use FHD folder."
                                 ));

    Spinbox_RefreshmentInterval->setSuffix(" " + tr("min") + ".");
    Spinbox_RefreshmentContinuous->setSuffix(" " + tr("sec") + ".");

    CheckBox_Clock->setText(tr("Clock"));
    CheckBox_Message->setText(tr("30-sec message"));
    CheckBox_Logging->setText(tr("Logging to .txt"));
    CheckBox_PrettyFont->setText(tr("PrettyFont"));
    CheckBox_PrettyFont->setToolTip(tr("UKIJ Diwani Yantu Font"));
    CheckBox_Text->setText(tr("Text") + ":");
    CheckBox_StartupLink->setText(tr("Run on Windows startup"));

    TextEdit_Text->setToolTip(tr("Support params: %interval, %continuous"));
    ButtonGenerateText->setText(tr("Other text"));

    Combobox_imageAspectMode->clear();
    Combobox_imageAspectMode->addItems(QStringList() << "" << "" << "");
    Combobox_imageAspectMode->setItemText(0, tr("Auto"));
    Combobox_imageAspectMode->setItemText(1, tr("Outside"));
    Combobox_imageAspectMode->setItemText(2, tr("Inside"));

    Combobox_iconsMode->clear();
    Combobox_iconsMode->addItems(QStringList() << "" << "");
    Combobox_iconsMode->setItemText(0, tr("Light"));
    Combobox_iconsMode->setItemText(1, tr("Dark"));

    GroupBox_BreakSetting->setTitle(tr("Settings") + ":");

    Label_ImageAspectMode->setText(tr("Image Aspect Mode") + ":");
    Label_UntilBreak->setText(tr("Until break") + ":");
    Label_BreakContinuous->setText(tr("Break continuous") + ":");

    GroupBox_SystemSetting->setTitle(tr("System Settings") + ":");
    Label_TrayIconStyle->setText(tr("Tray Icon Style") + ":");
    Label_Timer_Prefix->setText(tr("Until break") + ": ");

    buttonSave->setText(tr("Save"));
    buttonMinimizeToSystemTray->setText(tr("Close to notification area"));
}


void Dialog::showEvent(QShowEvent *e)
{
#ifdef Q_OS_WIN32
    TaskbarButton->setWindow(windowHandle());
#endif

    e->accept();
}




void Dialog::SetValues(const Setting &setting)
{
    this->setting = setting;

    Spinbox_RefreshmentInterval->setValue(setting.pauseInterval / 1000 / 60);
    Spinbox_RefreshmentContinuous->setValue(setting.pauseContinuous / 1000);
    LineEdit_Path->setText(setting.imagesPath);
    LineEdit_Path_alt->setText(setting.imagesPathAlternative);

    Combobox_imageAspectMode->setCurrentIndex(setting.imageAspectMode);
    Combobox_iconsMode->setCurrentIndex(setting.iconsMode);

    CheckBox_Logging->setChecked(setting.isLogging);
    CheckBox_Text->setChecked(setting.isText);
    CheckBox_Clock->setChecked(setting.isClock);
    CheckBox_Message->setChecked(setting.isMessage30sec);
    CheckBox_PrettyFont->setChecked(setting.isPrettyFont);
    CheckBox_StartupLink->setChecked(setting.isStartupLink);
    TextEdit_Text->setEnabled(setting.isText);
    ButtonGenerateText->setEnabled(setting.isText);
    TextEdit_Text->setPlainText(setting.text);


    emit TimerStatusRequest();
    InitConnectWidgetsChanged();

    /*connect(LineEdit_Path, SIGNAL(textChanged(QString)), this, SLOT(SaveValues()));
    connect(Combobox_imageAspectMode, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveValues()));
    connect(Spinbox_RefreshmentInterval, SIGNAL(valueChanged(int)), this, SLOT(SaveValues()));
    connect(Spinbox_RefreshmentContinuous, SIGNAL(valueChanged(int)), this, SLOT(SaveValues()));
    connect(CheckBox_Clock, SIGNAL(clicked(bool)), this, SLOT(SaveValues()));
    connect(CheckBox_Logging, SIGNAL(clicked(bool)), this, SLOT(SaveValues()));
    connect(CheckBox_Message, SIGNAL(clicked(bool)), this, SLOT(SaveValues()));
    connect(CheckBox_Text, SIGNAL(clicked(bool)), this, SLOT(SaveValues()));
    connect(CheckBox_PrettyFont, SIGNAL(clicked(bool)), this, SLOT(SaveValues()));
    */
    //connect(TextEdit_Text, SIGNAL(textChanged()), this, SLOT(SaveValues()));
}

void Dialog::UpdateLabel(const QString &time, const double &ratio)
{
    Label_Timer->setText(time);

    if (ratio >= 0) {
        TaskbarProgress->resume();
        TaskbarProgress->setValue(ratio * 100);

        //qDebug() << "Set time " << time;
    }
    else {
        TaskbarProgress->pause();
        TaskbarProgress->setValue(-ratio * 100);
        //qDebug() << "pause " << time;
    }


}

bool Dialog::event(QEvent *event)
{

    if (event->type() == QEvent::Close) {
        //event->accept();
        emit closedialog();
        return QDialog::close();
        //return QDialog::event(event);
    }
    else {
        //event->accept();
        return QDialog::event(event);
    }
}


void Dialog::ButtonPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                   (LineEdit_Path->text().isEmpty() ? QDir::homePath() : LineEdit_Path->text()), QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        LineEdit_Path->setText(path);
}

void Dialog::ButtonPath_alt_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                   (LineEdit_Path_alt->text().isEmpty() ? QDir::homePath() : LineEdit_Path_alt->text()), QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        LineEdit_Path_alt->setText(path);
}

void Dialog::ButtonGenerateText_clicked()
{
    QStringList proverbs = qApp->translate("App", "Proverbs.").split("\n\n");
    TextEdit_Text->setPlainText(proverbs[rand() % proverbs.size()]);


}

void Dialog::SaveValues()
{
    //emit TimerStatusRequest();
    //Label_Timer->setText(QString("%1:00").arg(Spinbox_RefreshmentInterval->value(),2,10,QLatin1Char(' ')));

    setting = Setting{
        setting.counter,
        Spinbox_RefreshmentInterval->value() * 60 * 1000,
        Spinbox_RefreshmentContinuous->value() * 1000,
        LineEdit_Path->text(),
        LineEdit_Path_alt->text(),
        static_cast<ImageAspectMode>(Combobox_imageAspectMode->currentIndex()),
        static_cast<IconsMode>(Combobox_iconsMode->currentIndex()),
        CheckBox_Logging->isChecked(),
        CheckBox_Text->isChecked(),
        CheckBox_Clock->isChecked(),
        CheckBox_Message->isChecked(),
        CheckBox_PrettyFont->isChecked(),
        CheckBox_StartupLink->isChecked(),
        TextEdit_Text->toPlainText()
    };

    emit save(setting);
}

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

Dialog::Dialog(): QDialog()
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

    TabWidget = new QTabWidget();
    TabWidget->setUsesScrollButtons(false);
    TopLayout->addWidget(TabWidget);

    QWidget * tab1 = new QWidget();
    QWidget * tab2 = new QWidget();
    TabWidget->addTab(tab1, "");
    TabWidget->addTab(tab2, "");

    QVBoxLayout *Tab1Layout = new QVBoxLayout();
    QVBoxLayout *Tab2Layout = new QVBoxLayout();
    //setLayout(TopLayout);

    tab1->setLayout(Tab1Layout);
    tab2->setLayout(Tab2Layout);

    Label_Path = new QLabel();
    LineEdit_Path = new QLineEdit();
    QPushButton *ButtonPath = new QPushButton("...");
    ButtonPath->setMaximumWidth(25);

    label_Path_alt = new QLabel();
    LineEdit_Path_alt = new QLineEdit();
    QPushButton *ButtonPath_alt = new QPushButton("...");
    ButtonPath_alt->setMaximumWidth(25);

    Spinbox_UntilBreak   = new QSpinBox();
    Spinbox_BreakDuration = new QSpinBox();
    Spinbox_UntilBreak->setRange(1, 24 * 60);
    Spinbox_BreakDuration->setRange(0, 60 * 60);


    CheckBox_Spectrum        = new QCheckBox();
    CheckBox_Tiling         = new QCheckBox();
    CheckBox_Random_circle  = new QCheckBox();
    CheckBox_Random_circles = new QCheckBox();
    CheckBox_Neo            = new QCheckBox();
    CheckBox_Stripes        = new QCheckBox();


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
    Label_BreakDuration = new QLabel();
    Label_TrayIconStyle = new QLabel();

    buttonSave = new QPushButton();
    buttonSave->setIcon(QIcon(":/icons/save.png"));
    buttonSave->setEnabled(false);
    buttonMinimizeToSystemTray = new QPushButton();
    buttonSave->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    buttonMinimizeToSystemTray->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    //----------- tab1

    GroupBox_Tab1 = new QGroupBox();
    QGridLayout *layoutGrid_Tab1 = new QGridLayout();
    GroupBox_Tab1->setLayout(layoutGrid_Tab1);

    layoutGrid_Tab1->addWidget(Label_UntilBreak,              1, 0);
    layoutGrid_Tab1->addWidget(Spinbox_UntilBreak,   1, 1);
    layoutGrid_Tab1->addWidget(Label_BreakDuration,         2, 0);
    layoutGrid_Tab1->addWidget(Spinbox_BreakDuration, 2, 1);
    layoutGrid_Tab1->addWidget(CheckBox_Message,        3, 0, 1, 2);

    layoutGrid_Tab1->setColumnStretch(0, 2);
    layoutGrid_Tab1->setColumnStretch(1, 1);

    Tab1Layout->addWidget(GroupBox_Tab1);

    Tab1Layout->addSpacing(30);

    // --
    GroupBox_SystemSetting = new QGroupBox();
    QGridLayout *layoutGrid_SystemSetting = new QGridLayout();
    GroupBox_SystemSetting->setLayout(layoutGrid_SystemSetting);

    layoutGrid_SystemSetting->addWidget(Label_TrayIconStyle,  0, 0);
    layoutGrid_SystemSetting->addWidget(Combobox_iconsMode,   0, 1);
    layoutGrid_SystemSetting->addWidget(CheckBox_StartupLink, 2, 0, 1, 2);

    layoutGrid_SystemSetting->addWidget(CheckBox_Logging,     4, 0, 1, 2);

    layoutGrid_SystemSetting->setColumnStretch(0, 2);
    layoutGrid_SystemSetting->setColumnStretch(1, 1);

    Tab1Layout->addWidget(GroupBox_SystemSetting);

    Tab1Layout->addStretch();

    //------------- tab2

    GroupBox_Background = new QGroupBox();
    QVBoxLayout *Layout_Background = new QVBoxLayout();
    GroupBox_Background->setLayout(Layout_Background);

    QHBoxLayout *layout_Path = new QHBoxLayout();
    layout_Path->addWidget(LineEdit_Path);
    layout_Path->addWidget(ButtonPath);

    QHBoxLayout *layout_Path_alt = new QHBoxLayout();
    layout_Path_alt->addWidget(LineEdit_Path_alt);
    layout_Path_alt->addWidget(ButtonPath_alt);

    Layout_Background->addWidget(Label_Path);
    Layout_Background->addLayout(layout_Path);
    Layout_Background->addWidget(label_Path_alt);
    Layout_Background->addLayout(layout_Path_alt);

    QGridLayout *layoutGrid_Background = new QGridLayout();
    Layout_Background->addLayout(layoutGrid_Background);
    layoutGrid_Background->addWidget(Label_ImageAspectMode,         0, 0);
    layoutGrid_Background->addWidget(Combobox_imageAspectMode,      0, 1);
    layoutGrid_Background->setColumnStretch(0, 2);
    layoutGrid_Background->setColumnStretch(1, 1);


    GroupBox_PredeterminedBackground = new QGroupBox();
    QGridLayout *layout_PredeterminedBackground = new QGridLayout();
    GroupBox_PredeterminedBackground->setLayout(layout_PredeterminedBackground);

    layout_PredeterminedBackground->addWidget(CheckBox_Spectrum       ,3,0);
    layout_PredeterminedBackground->addWidget(CheckBox_Tiling         ,4,0);
    layout_PredeterminedBackground->addWidget(CheckBox_Stripes        ,5,0);
    layout_PredeterminedBackground->addWidget(CheckBox_Random_circle  ,3,1);
    layout_PredeterminedBackground->addWidget(CheckBox_Random_circles ,4,1);
    layout_PredeterminedBackground->addWidget(CheckBox_Neo            ,5,1);

    GroupBox_Foreground = new QGroupBox();
    QGridLayout *layout_Foreground = new QGridLayout();
    GroupBox_Foreground->setLayout(layout_Foreground);

    layout_Foreground->addWidget(CheckBox_PrettyFont,  7, 0, 1, 2);
    layout_Foreground->addWidget(CheckBox_Clock,       8, 0, 1, 2);
    layout_Foreground->addWidget(CheckBox_Text,        9, 0);
    layout_Foreground->addWidget(ButtonGenerateText,   9, 1);
    layout_Foreground->addWidget(TextEdit_Text,        10, 0, 1, 2);

    layout_Foreground->setColumnStretch(0, 2);
    layout_Foreground->setColumnStretch(1, 1);

    Tab2Layout->addWidget(GroupBox_Background);
    Tab2Layout->addSpacing(15);
    Tab2Layout->addWidget(GroupBox_PredeterminedBackground);
    Tab2Layout->addStretch();
    Tab2Layout->addSpacing(30);
    Tab2Layout->addWidget(GroupBox_Foreground);

    //----------

    TopLayout->addStretch();
    TopLayout->addSpacing(15);

    QHBoxLayout *layout_Label_timer = new QHBoxLayout();

    Label_Timer_Prefix = new QLabel();

    Label_Timer = new QLabel();
    QFont font;
    font.setPointSize(16);
    Label_Timer->setFont(font);
    Label_Timer->setAlignment(Qt::AlignLeft);

    //Label_Timer_Prefix->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //Label_Timer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

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

    connect(CheckBox_Text, SIGNAL(toggled(bool)), this, SLOT(CheckBoxPrettyFont_Enable()));
    connect(CheckBox_Clock, SIGNAL(toggled(bool)), this, SLOT(CheckBoxPrettyFont_Enable()));

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

    connect(Spinbox_UntilBreak,   SIGNAL(valueChanged(int)), this, SLOT(SaveButton_status()));
    connect(Spinbox_BreakDuration, SIGNAL(valueChanged(int)), this, SLOT(SaveButton_status()));

    connect(CheckBox_Clock,       SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Message,     SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Logging,     SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_PrettyFont,  SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Text,        SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_StartupLink, SIGNAL(clicked()), this, SLOT(SaveButton_status()));

    connect(CheckBox_Spectrum      , SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Tiling        , SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Stripes       , SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Random_circle , SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Random_circles, SIGNAL(clicked()), this, SLOT(SaveButton_status()));
    connect(CheckBox_Neo           , SIGNAL(clicked()), this, SLOT(SaveButton_status()));
}

void Dialog::CheckBoxPrettyFont_Enable(){
    if (!CheckBox_Text->isChecked() && !CheckBox_Clock->isChecked())
        CheckBox_PrettyFont->setEnabled(false);
    else
        CheckBox_PrettyFont->setEnabled(true);

}
void Dialog::SaveButton_status()
{
    Setting s = {
        setting.running_counter,
        Spinbox_UntilBreak->value() * 60 * 1000,
        Spinbox_BreakDuration->value() * 1000,
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
        TextEdit_Text->toPlainText(),
        CheckBox_Spectrum->isChecked(),
        CheckBox_Tiling->isChecked(),
        CheckBox_Stripes       ->isChecked(),
        CheckBox_Random_circle ->isChecked(),
        CheckBox_Random_circles->isChecked(),
        CheckBox_Neo           ->isChecked()
    };

    if (setting != s)
        buttonSave->setEnabled(true);
    else
        buttonSave->setEnabled(false);
}


void Dialog::Translate()
{
    // mutual
    setWindowTitle(qApp->translate("App", "Eyes' Thanks"));
    Label_Timer_Prefix->setText(tr("Until break") + ":");
    buttonSave->setText(tr("Save"));
    buttonMinimizeToSystemTray->setText(tr("Close to notification area"));
    TabWidget->setTabText(0, tr("Program Setting"));
    TabWidget->setTabText(1, tr("Showing Break Setting"));

    // tab1
    Label_UntilBreak->setText(tr("Until break") + ":");
    Label_BreakDuration->setText(tr("Break duration") + ":");
    Spinbox_UntilBreak->setSuffix(" " + tr("min") + ".");
    Spinbox_BreakDuration->setSuffix(" " + tr("sec") + ".");


    GroupBox_SystemSetting->setTitle(tr("System Settings") + ":");
    Label_TrayIconStyle->setText(tr("Tray Icon Style") + ":");
    Combobox_iconsMode->clear();
    Combobox_iconsMode->addItems(QStringList() << "" << "");
    Combobox_iconsMode->setItemText(0, tr("Light"));
    Combobox_iconsMode->setItemText(1, tr("Dark"));

    CheckBox_Message->setText(tr("30-sec message"));
    CheckBox_Logging->setText(tr("Logging to .txt"));
    CheckBox_StartupLink->setText(tr("Run on Windows startup"));

    // tab2
    GroupBox_Background->setTitle(tr("User Backgrounds")+ ":");

    Label_Path->setText(tr("Pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    Label_Path->setToolTip(tr("Choose folder with images (jpg, png, gif), or leave empty for Predetermined Backgrounds."));

    label_Path_alt->setText(tr("Alternative pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    label_Path_alt->setToolTip(tr("Use, if you have two different-ratio sets of monitors.\n\n"
                                  "Ex. if sometimes you disconnect your FHD notebook from your FHD monitor,\n"
                                  "put 2FHD (3860×1080) pictures to “Pictures folder” and\n"
                                  "FHD (1920×1080) pictures to “Alternative pictures folder”).\n"
                                  "So, connect laptop to monitor — app use 2FHD folder, laptop by itself — app use FHD folder."
                                 ));

    GroupBox_PredeterminedBackground->setTitle(tr("Predetermined Backgrounds")+":");
    CheckBox_Spectrum       ->setText(tr("Spectrum"));
    CheckBox_Tiling         ->setText(tr("Tiling"));
    CheckBox_Stripes        ->setText(tr("Stripes"));
    CheckBox_Random_circle  ->setText(tr("Circle"));
    CheckBox_Random_circles ->setText(tr("Circles"));
    CheckBox_Neo            ->setText(tr("Neo"));


    GroupBox_Foreground->setTitle(tr("Foreground")+ ":");
    CheckBox_Clock->setText(tr("Clock"));
    CheckBox_PrettyFont->setText(tr("Pretty Font"));
    CheckBox_PrettyFont->setToolTip(tr("UKIJ Diwani Yantu Mod Font"));
    CheckBox_Text->setText(tr("Text") + ":");
    TextEdit_Text->setToolTip(tr("Support params")+": %until, %duration");
    ButtonGenerateText->setText(tr("Random proverb"));

    Combobox_imageAspectMode->clear();
    Combobox_imageAspectMode->addItems(QStringList() << "" << "" << "");
    Combobox_imageAspectMode->setItemText(0, tr("Auto"));
    Combobox_imageAspectMode->setItemText(1, tr("Outside"));
    Combobox_imageAspectMode->setItemText(2, tr("Inside"));

    //GroupBox_BreakSetting->setTitle(tr("Settings") + ":");

    Label_ImageAspectMode->setText(tr("Image Aspect Mode") + ":");
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

    Spinbox_UntilBreak->setValue(setting.pauseInterval / 1000 / 60);
    Spinbox_BreakDuration->setValue(setting.pauseDuration / 1000);
    LineEdit_Path->setText(setting.imagesPath);
    LineEdit_Path_alt->setText(setting.imagesPathAlternative);

    Combobox_imageAspectMode->setCurrentIndex(setting.imageAspectMode);
    Combobox_iconsMode->setCurrentIndex(setting.iconsMode);


    CheckBox_Spectrum      ->setChecked(setting.isSpectrum      );
    CheckBox_Tiling        ->setChecked(setting.isTiling        );
    CheckBox_Stripes       ->setChecked(setting.isStripes       );
    CheckBox_Random_circle ->setChecked(setting.isCircle );
    CheckBox_Random_circles->setChecked(setting.isCircles);
    CheckBox_Neo           ->setChecked(setting.isNeo           );

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
}

void Dialog::UpdateLabel(const QString &time, const qreal &ratio)
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
    TextEdit_Text->setPlainText(proverbs[qrand() % proverbs.size()]);


}

void Dialog::SaveValues()
{
    //emit TimerStatusRequest();
    //Label_Timer->setText(QString("%1:00").arg(Spinbox_RefreshmentInterval->value(),2,10,QLatin1Char(' ')));

    setting = Setting{
        setting.running_counter,
        Spinbox_UntilBreak->value() * 60 * 1000,
        Spinbox_BreakDuration->value() * 1000,
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
        TextEdit_Text->toPlainText(),
        CheckBox_Spectrum      ->isChecked(),
        CheckBox_Tiling        ->isChecked(),
        CheckBox_Stripes       ->isChecked(),
        CheckBox_Random_circle ->isChecked(),
        CheckBox_Random_circles->isChecked(),
        CheckBox_Neo           ->isChecked()
    };

    emit save(setting);
}

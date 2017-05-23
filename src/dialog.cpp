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

#include "dialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>

Dialog::Dialog()
{
    setWindowTitle(QWidget::tr("Eyes' Thanks"));
    setWindowIcon(QIcon(":icons/logo.png"));



#ifdef _WIN32
    //setWindowFlags( Qt::WindowTitleHint | Qt::CustomizeWindowHint); //-- no title buttons
    //setWindowFlags(Qt::WindowCloseButtonHint);
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

#endif

    Init();

}

void Dialog::Init()
{
    QVBoxLayout *TopLayout = new QVBoxLayout();
    setLayout(TopLayout);

    QLabel *label_Path = new QLabel(tr("Pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    LineEdit_Path = new QLineEdit();
    QPushButton *ButtonPath = new QPushButton("...");
    ButtonPath->setMaximumWidth(25);
    QString pic_folder_tooltip = QString(tr("Leave both folder paths empty, if you want nice gradient instead of pictures."));
    label_Path->setToolTip(pic_folder_tooltip);

    QLabel *label_Path_alt = new QLabel(tr("Alternative pictures folder") + "<img src=':/icons/tooltip.png'>" ":");
    LineEdit_Path_alt = new QLineEdit();
    QPushButton *ButtonPath_alt = new QPushButton("...");
    ButtonPath_alt->setMaximumWidth(25);
    QString alt_pic_folder = QString(tr("Use, if you have two different-ratio sets of monitors.\n\n"
                                        "Ex. if sometimes you disconnect your FHD notebook from your FHD monitor,\n"
                                        "put 2FHD (3860×1080) pictures to “Pictures folder” and\n"
                                        "FHD (1920×1080) pictures to “Alternative pictures folder”).\n"
                                        "So, connect laptop to monitor — app use 2FHD folder, laptop by itself — app use FHD folder."
                                        ));
    label_Path_alt->setToolTip(alt_pic_folder);

    Spinbox_RefreshmentInterval   = new QSpinBox();
    Spinbox_RefreshmentContinuous = new QSpinBox();
    Spinbox_RefreshmentInterval->setRange(1, 24 * 60);
    Spinbox_RefreshmentContinuous->setRange(0, 60 * 60);
    Spinbox_RefreshmentInterval->setSuffix(" " + tr("min") + ".");
    Spinbox_RefreshmentContinuous->setSuffix(" " + tr("sec")+ ".");

    CheckBox_Clock       = new QCheckBox(tr("Clock"));
    CheckBox_Message     = new QCheckBox(tr("30-sec message"));
    CheckBox_Logging     = new QCheckBox(tr("Logging to .txt"));
    CheckBox_PrettyFont   = new QCheckBox(tr("PrettyFont"));
    CheckBox_Text        = new QCheckBox(tr("Text")+ ":");
    CheckBox_StartupLink = new QCheckBox(tr("Run on Windows startup"));

    TextEdit_Text = new QPlainTextEdit();
    TextEdit_Text->setFixedHeight(70);
    TextEdit_Text->setToolTip(tr("Support params: %interval, %continuous"));

    Combobox_imageAspectMode = new QComboBox();
    Combobox_imageAspectMode->insertItems(0, QStringList() << QWidget::tr("Auto") << QWidget::tr("Outside") << QWidget::tr("Inside"));

    Combobox_iconsMode = new QComboBox();
    Combobox_iconsMode->insertItems(0, QStringList() << tr("Light") << tr("Dark"));


    QGroupBox *SettingBreakShowing = new QGroupBox(tr("Settings") + ":");
    QVBoxLayout *layoutVertical_GroupBox = new QVBoxLayout();
    SettingBreakShowing->setLayout(layoutVertical_GroupBox);


    QHBoxLayout *layout_Path = new QHBoxLayout();
    layout_Path->addWidget(LineEdit_Path);
    layout_Path->addWidget(ButtonPath);

    QHBoxLayout *layout_Path_alt = new QHBoxLayout();
    layout_Path_alt->addWidget(LineEdit_Path_alt);
    layout_Path_alt->addWidget(ButtonPath_alt);

    layoutVertical_GroupBox->addWidget(label_Path);
    layoutVertical_GroupBox->addLayout(layout_Path);

    layoutVertical_GroupBox->addWidget(label_Path_alt);
    layoutVertical_GroupBox->addLayout(layout_Path_alt);

    //-----
    QGridLayout *layoutGrid_SettingBreakShowing = new QGridLayout();
    layoutGrid_SettingBreakShowing->addWidget(new QLabel(tr("Image Aspect Mode") + ":"), 0, 0);
    layoutGrid_SettingBreakShowing->addWidget(Combobox_imageAspectMode, 0, 1);
    layoutGrid_SettingBreakShowing->addWidget(new QLabel(tr("Until break") + ":"), 1, 0);
    layoutGrid_SettingBreakShowing->addWidget(Spinbox_RefreshmentInterval, 1, 1);
    layoutGrid_SettingBreakShowing->addWidget(new QLabel(tr("Break continuous") + ":"), 2, 0);
    layoutGrid_SettingBreakShowing->addWidget(Spinbox_RefreshmentContinuous, 2, 1);
    layoutGrid_SettingBreakShowing->addWidget(CheckBox_PrettyFont, 7, 0);
    layoutGrid_SettingBreakShowing->addWidget(CheckBox_Clock, 8, 0);
    layoutGrid_SettingBreakShowing->addWidget(CheckBox_Text, 9, 0);
    layoutGrid_SettingBreakShowing->addWidget(TextEdit_Text, 10, 0, 1, 2);

    layoutVertical_GroupBox->addLayout(layoutGrid_SettingBreakShowing);
    TopLayout->addWidget(SettingBreakShowing);


    TopLayout->addSpacing(30);
    TopLayout->addStretch();


    //-----
    QGroupBox *SettingSystem = new QGroupBox(tr("System Settings") + ":");
    QGridLayout *Layout_SettingSystem = new QGridLayout();

    Layout_SettingSystem->addWidget(new QLabel(tr("Tray Icon Style") + ":"), 0, 0);
    Layout_SettingSystem->addWidget(Combobox_iconsMode, 0, 1);
    Layout_SettingSystem->addWidget(CheckBox_StartupLink, 2, 0, 1, 2);
    Layout_SettingSystem->addWidget(CheckBox_Message, 3, 0, 1, 2);
    Layout_SettingSystem->addWidget(CheckBox_Logging, 4, 0);

    SettingSystem->setLayout(Layout_SettingSystem);
    TopLayout->addWidget(SettingSystem);
    TopLayout->addStretch();
    TopLayout->addSpacing(15);

    //-----
    QHBoxLayout *Layout_Label_timer = new QHBoxLayout();

    QLabel *Label_Timer_Prefix = new QLabel(tr("Until break") + ": ");

    Label_Timer = new QLabel();
    QFont font;
    font.setPointSize(16);
    Label_Timer->setFont(font);
    Label_Timer->setAlignment(Qt::AlignCenter);
    Layout_Label_timer->addWidget(Label_Timer_Prefix);
    Layout_Label_timer->addWidget(Label_Timer);
    Label_Timer_Prefix->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    Label_Timer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    TopLayout->addLayout(Layout_Label_timer);
    TopLayout->addStretch();
    TopLayout->addSpacing(15);

    //-----
    QHBoxLayout *layout_buttons = new QHBoxLayout();

    QPushButton *buttonSave = new QPushButton( tr("Save"));
    buttonSave->setIcon(QIcon(":/icons/save.png"));
    layout_buttons->addWidget(buttonSave);

    QPushButton *buttonMinimizeToSystemTray = new QPushButton(tr("Close to notification area"));
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
    connect(CheckBox_Text, SIGNAL(clicked(bool)), TextEdit_Text, SLOT(setEnabled(bool)));
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(SaveValues()));
    connect(buttonMinimizeToSystemTray, SIGNAL(clicked()), this, SLOT(close()));
}


void Dialog::showEvent(QShowEvent *e)
{
#ifdef Q_OS_WIN32
    TaskbarButton->setWindow(windowHandle());
#endif

    e->accept();
}




void Dialog::SetValues(int pauseInterval, int pauseContinuous, QString ImagesPath, QString ImagesPath_alt, QString imageAspectMode,
                       bool isLogging, bool isText, bool isClock, bool isMessage30sec, bool isPrettyFont, bool isStartupLink, QString Text, IconsMode iconsmode)
{
    Spinbox_RefreshmentInterval->setValue(pauseInterval / 1000 / 60);
    Spinbox_RefreshmentContinuous->setValue(pauseContinuous / 1000);
    LineEdit_Path->setText(ImagesPath);
    LineEdit_Path_alt->setText(ImagesPath_alt);
    Combobox_imageAspectMode->setCurrentText(imageAspectMode);
    Combobox_iconsMode->setCurrentIndex(iconsmode);
    CheckBox_Logging->setChecked(isLogging);
    CheckBox_Text->setChecked(isText);
    CheckBox_Clock->setChecked(isClock);
    CheckBox_Message->setChecked(isMessage30sec);
    CheckBox_PrettyFont->setChecked(isPrettyFont);
    CheckBox_StartupLink->setChecked(isStartupLink);
    TextEdit_Text->setEnabled(isText);
    TextEdit_Text->setPlainText(Text);


    emit TimerStatusRequest();

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

void Dialog::UpdateLabel(QString time, double ratio)
{
    Label_Timer->setText(time);

    if (ratio >= 0) {
        TaskbarProgress->resume();
        TaskbarProgress->setValue(ratio * 100);

        //qDebug() << "Set time " << time;
    } else {
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
    } else {
        //event->accept();
        return QDialog::event(event);
    }
}


void Dialog::ButtonPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                   (LineEdit_Path->text().isEmpty() ? QDir::homePath() : LineEdit_Path->text()), QFileDialog::ShowDirsOnly);
    if (!path.isEmpty()) {
        LineEdit_Path->setText(path);
    }
    LineEdit_Path->resize(LineEdit_Path->sizeHint());
    resize(sizeHint());
}

void Dialog::ButtonPath_alt_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                   (LineEdit_Path_alt->text().isEmpty() ? QDir::homePath() : LineEdit_Path_alt->text()), QFileDialog::ShowDirsOnly);
    if (!path.isEmpty()) {
        LineEdit_Path_alt->setText(path);
    }
}

void Dialog::SaveValues()
{
    //emit TimerStatusRequest();
    //Label_Timer->setText(QString("%1:00").arg(Spinbox_RefreshmentInterval->value(),2,10,QLatin1Char(' ')));

    emit save(Spinbox_RefreshmentInterval->value() * 60 * 1000, Spinbox_RefreshmentContinuous->value() * 1000,
              LineEdit_Path->text(), LineEdit_Path_alt->text(), Combobox_imageAspectMode->currentText(),
              CheckBox_Logging->isChecked(), CheckBox_Text->isChecked(), CheckBox_Clock->isChecked(), CheckBox_Message->isChecked(),
              CheckBox_PrettyFont->isChecked(), CheckBox_StartupLink->isChecked(), TextEdit_Text->toPlainText(), static_cast<IconsMode>(Combobox_iconsMode->currentIndex()));
}

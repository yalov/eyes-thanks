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
    setWindowTitle(tr("Eyes' Thanks"));
    setWindowIcon(QIcon(":icons/logo.png"));



#ifdef _WIN32
    //setWindowFlags( Qt::WindowTitleHint | Qt::CustomizeWindowHint); //-- no title buttons
    //setWindowFlags(Qt::WindowCloseButtonHint);
    setWindowFlags( Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

#endif

    Init();

}

void Dialog::Init()
{
    QVBoxLayout * TopLayout = new QVBoxLayout();
    setLayout(TopLayout);

    QHBoxLayout * layout_Path = new QHBoxLayout();
    QLabel * label_path = new QLabel(tr("Pictures folder") + ":");
    label_path->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    LineEdit_Path = new QLineEdit();
    //LineEdit_Path->setReadOnly(true);
    layout_Path->addWidget(LineEdit_Path);

    QPushButton * ButtonPath = new QPushButton("...");
    //ButtonPath->setIcon(QWidget().style()->standardIcon(QStyle::SP_DirOpenIcon));
    ButtonPath->setMaximumWidth(25);
    layout_Path->addWidget(ButtonPath);

    TopLayout->addWidget(label_path);
    TopLayout->addLayout(layout_Path);

    QHBoxLayout * layout_AspectMode = new QHBoxLayout();
    Combobox_imageAspectMode = new QComboBox();
    Combobox_imageAspectMode->insertItems(0, QStringList() << tr("Auto") << tr("Outside") << tr("Inside"));
    layout_AspectMode->addWidget(new QLabel(tr("Image Aspect Mode")+ ":" ));
    layout_AspectMode->addWidget(Combobox_imageAspectMode);
    TopLayout->addLayout(layout_AspectMode);


    QHBoxLayout * layout_iconsMode = new QHBoxLayout();
    Combobox_iconsMode = new QComboBox();
    Combobox_iconsMode->insertItems(0, QStringList() << tr("Light") << tr("Dark"));
    layout_iconsMode->addWidget(new QLabel(tr("Tray Icon Style")+ ":" ));
    layout_iconsMode->addWidget(Combobox_iconsMode);
    TopLayout->addLayout(layout_iconsMode);


    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    TopLayout->addWidget(line);


    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);


    //TopLayout->addStretch();


    Spinbox_RefreshmentInterval   = new QSpinBox();
    Spinbox_RefreshmentContinuous = new QSpinBox();
    Spinbox_RefreshmentInterval->setRange  (1,24*60);
    Spinbox_RefreshmentContinuous->setRange(0,60*60);
    Spinbox_RefreshmentInterval->setSuffix  (" " + tr("min"));
    Spinbox_RefreshmentContinuous->setSuffix(" " + tr("sec"));

    CheckBox_Clock    = new QCheckBox(tr("Clock"));
    CheckBox_Message  = new QCheckBox(tr("30-sec message"));
    CheckBox_Logging    = new QCheckBox(tr("Logging"));
    CheckBox_PrettyFont   = new QCheckBox(tr("PrettyFont"));
    CheckBox_Text     = new QCheckBox(tr("Text"));


    QGridLayout * Layout_TimeIntervalSetting = new QGridLayout();
    Layout_TimeIntervalSetting->addWidget(new QLabel(tr("Until break")), 0,0);
    Layout_TimeIntervalSetting->addWidget(Spinbox_RefreshmentInterval, 0,1);
    Layout_TimeIntervalSetting->addWidget(new QLabel(tr("Break continuous")), 1,0);
    Layout_TimeIntervalSetting->addWidget(Spinbox_RefreshmentContinuous, 1,1);


    Layout_TimeIntervalSetting->addWidget(CheckBox_Message, 2,0,1,2);

    Layout_TimeIntervalSetting->addWidget(CheckBox_Logging, 3,0);

    Layout_TimeIntervalSetting->addWidget(line2, 4,0,1,2);

    Layout_TimeIntervalSetting->addWidget(CheckBox_PrettyFont, 5,0);
    Layout_TimeIntervalSetting->addWidget(CheckBox_Clock, 6,0);
    Layout_TimeIntervalSetting->addWidget(CheckBox_Text, 6,1);

    TopLayout->addLayout(Layout_TimeIntervalSetting);


    TextEdit_Text = new QPlainTextEdit();
    //TextEdit_Text->setMaximumHeight(70);
    TextEdit_Text->setFixedHeight(70);

    TextEdit_Text->setToolTip(tr("Support params: %interval, %continuous"));
    TopLayout->addWidget(TextEdit_Text);
    TextEdit_Text->setGeometry(0,0,50,50);

    QHBoxLayout * Layout_Label_timer = new QHBoxLayout();

    QLabel * Label_Timer_Prefix = new QLabel(tr("Until break") + ": ");

    Label_Timer = new QLabel();
    QFont font; font.setPointSize(16);
    Label_Timer->setFont(font);
    Label_Timer->setAlignment(Qt::AlignCenter);
    Layout_Label_timer->addWidget(Label_Timer_Prefix);
    Layout_Label_timer->addWidget(Label_Timer);
    TopLayout->addLayout(Layout_Label_timer);
    Label_Timer_Prefix->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    Label_Timer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout * layout_buttons = new QHBoxLayout();

    QPushButton * buttonSave = new QPushButton(tr("Save"));
    layout_buttons->addWidget(buttonSave);

    QPushButton * buttonMinimizeToSystemTray = new QPushButton(tr("Close to notification area"));
    layout_buttons->addWidget(buttonMinimizeToSystemTray);

    TopLayout->addLayout(layout_buttons);



#ifdef Q_OS_WIN32


    TaskbarButton = new QWinTaskbarButton(this);
    //TaskbarButton->setOverlayIcon(QIcon(":icons/50.png"));

    TaskbarProgress = TaskbarButton->progress();
    TaskbarProgress->setVisible(true);


#endif

    connect(ButtonPath, SIGNAL(clicked()), this, SLOT(ButtonPath_clicked()));
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




void Dialog::SetValues(int pauseInterval, int pauseContinuous, QString ImagesPath, QString imageAspectMode,
                        bool isLogging, bool isText, bool isClock, bool isMessage30sec, bool isPrettyFont, QString Text, IconsMode iconsmode)
{
    Spinbox_RefreshmentInterval->setValue(pauseInterval /1000/60);
    Spinbox_RefreshmentContinuous->setValue(pauseContinuous/1000);
    LineEdit_Path->setText(ImagesPath);
    Combobox_imageAspectMode->setCurrentText(imageAspectMode);
    Combobox_iconsMode->setCurrentIndex(iconsmode);
    CheckBox_Logging->setChecked(isLogging);
    CheckBox_Text->setChecked(isText);
    CheckBox_Clock->setChecked(isClock);
    CheckBox_Message->setChecked(isMessage30sec);
    CheckBox_PrettyFont->setChecked(isPrettyFont);
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

    if (ratio>=0)
    {
        TaskbarProgress->resume();
        TaskbarProgress->setValue(ratio*100);

        //qDebug() << "Set time " << time;
    }
    else
    {
        TaskbarProgress->pause();
        TaskbarProgress->setValue(-ratio*100);
        //qDebug() << "pause " << time;
    }


}

bool Dialog::event(QEvent *event)
{

    if (event->type() == QEvent::Close)
    {
        //event->accept();
        emit closedialog();
        return QDialog::close();
        //return QDialog::event(event);
    }
    else
    {
        //event->accept();
        return QDialog::event(event);
    }
}


void Dialog::ButtonPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                   (LineEdit_Path->text().isEmpty()?QDir::homePath():LineEdit_Path->text()) , QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
    {
        LineEdit_Path->setText(path);
    }
}

void Dialog::SaveValues()
{
    //emit TimerStatusRequest();
    //Label_Timer->setText(QString("%1:00").arg(Spinbox_RefreshmentInterval->value(),2,10,QLatin1Char(' ')));

    emit save(Spinbox_RefreshmentInterval->value()*60 * 1000, Spinbox_RefreshmentContinuous->value() * 1000,
              LineEdit_Path->text(), Combobox_imageAspectMode->currentText(),
              CheckBox_Logging->isChecked(), CheckBox_Text->isChecked(), CheckBox_Clock->isChecked(), CheckBox_Message->isChecked(),
              CheckBox_PrettyFont->isChecked(), TextEdit_Text->toPlainText(), static_cast<IconsMode>(Combobox_iconsMode->currentIndex()));
}

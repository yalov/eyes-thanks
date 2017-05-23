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

#ifndef DIALOG_H
#define DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDebug>
#include <QGroupBox>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>


enum IconsMode {light, dark };

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();
    void SetValues(int pauseInterval, int pauseContinuous, QString ImagesPath, QString ImagesPath_alt, QString imageAspectMode, bool isLogging,
                   bool isText, bool isClock, bool isMessage30sec, bool isPrettyFont, bool isStartupLink, QString Text,  IconsMode iconsmode);

private:
    void showEvent(QShowEvent *e);
    bool event(QEvent *event);
    void Init();



public slots:
    void UpdateLabel(QString time, double ratio);

private slots:
    void SaveValues();
    void ButtonPath_clicked();
    void ButtonPath_alt_clicked();


signals:
    void closedialog();
    void save(int pauseInterval, int pauseContinuous, QString ImagesPath, QString ImagesPath_alt, QString imageAspectMode, bool isLogging,
              bool isText, bool isClock, bool isMessage30sec, bool isPrettyFont, bool isStartupLink, QString Text, IconsMode iconmode);
    void TimerStatusRequest();

private:
    QLineEdit * LineEdit_Path;
    QLineEdit * LineEdit_Path_alt;
    QPlainTextEdit * TextEdit_Text;
    QComboBox * Combobox_imageAspectMode;
    QComboBox * Combobox_iconsMode;
    QSpinBox * Spinbox_RefreshmentInterval;
    QSpinBox * Spinbox_RefreshmentContinuous;
    QLabel * Label_Timer;
    QCheckBox * CheckBox_Clock;
    QCheckBox * CheckBox_Message;
    QCheckBox * CheckBox_Logging;
    QCheckBox * CheckBox_PrettyFont;
    QCheckBox * CheckBox_Text;
    QCheckBox * CheckBox_StartupLink;

    QWinTaskbarButton *TaskbarButton;
    QWinTaskbarProgress *TaskbarProgress;
};

#endif // DIALOG_H

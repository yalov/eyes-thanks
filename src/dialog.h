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


class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();
    void Init();
    bool event(QEvent *event);
    void SetValues(int pauseInterval, int pauseContinuous, QString ImagesPath, QString imageAspectMode, bool isDebug,
                   bool isText, bool isClock, bool isMessage30sec, QString Text, int counter);


public slots:
    void UpdateLabel(QString time);


signals:
    void closedialog(bool minimized);
    void save(int pauseInterval, int pauseContinuous, QString ImagesPath, QString imageAspectMode, bool isDebug,
              bool isText, bool isClock, bool isMessage30sec, QString Text);


private slots:
    void SaveValues();
    void ButtonPath_clicked();

private:
    int Counter;
    QLineEdit * LineEdit_Path;
    QPlainTextEdit * TextEdit_Text;
    QComboBox * Combobox_imageAspectMode;
    QSpinBox * Spinbox_RefreshmentInterval;
    QSpinBox * Spinbox_RefreshmentContinuous;
    QLabel * Label_Timer;
    QCheckBox * CheckBox_Clock;
    QCheckBox * CheckBox_Text;
    QCheckBox * CheckBox_Message;
    QCheckBox * CheckBox_Debug;
};

#endif // DIALOG_H

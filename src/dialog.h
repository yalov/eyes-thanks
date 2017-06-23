//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
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

#include "global.h"


class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();
    void SetValues(Setting setting);
    void Translate();

private:
    void showEvent(QShowEvent *e);
    bool event(QEvent *event);
    void InitWidgets();
    void InitConnectWidgetsChanged();

public slots:
    void UpdateLabel(QString time, double ratio);

private slots:
    void SaveValues();
    void ButtonPath_clicked();
    void ButtonPath_alt_clicked();
    void ButtonGenerateText_clicked();
    void SaveButton_status();
signals:
    void closedialog();
    void save(Setting setting);
    void TimerStatusRequest();

private:
    Setting setting;

    QLabel *Label_Path;
    QLabel *label_Path_alt;
    QGroupBox *GroupBox_BreakSetting;
    QLabel *Label_ImageAspectMode;
    QLabel *Label_UntilBreak     ;
    QLabel *Label_BreakContinuous;
    QGroupBox *GroupBox_SystemSetting;
    QLabel *Label_TrayIconStyle;
    QLabel *Label_Timer_Prefix;
    QPushButton *buttonSave;
    QPushButton *buttonMinimizeToSystemTray;
    QLineEdit *LineEdit_Path;
    QLineEdit *LineEdit_Path_alt;
    QPlainTextEdit *TextEdit_Text;
    QComboBox *Combobox_imageAspectMode;
    QComboBox *Combobox_iconsMode;
    QSpinBox *Spinbox_RefreshmentInterval;
    QSpinBox *Spinbox_RefreshmentContinuous;
    QLabel *Label_Timer;
    QCheckBox *CheckBox_Clock;
    QCheckBox *CheckBox_Message;
    QCheckBox *CheckBox_Logging;
    QCheckBox *CheckBox_PrettyFont;
    QCheckBox *CheckBox_Text;
    QCheckBox *CheckBox_StartupLink;
    QPushButton *ButtonGenerateText;

    QWinTaskbarButton *TaskbarButton;
    QWinTaskbarProgress *TaskbarProgress;
};

#endif // DIALOG_H

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
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>

#include "global.h"


class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();
    void SetValues(const Setting &setting);
    void Translate();

private:
    void showEvent(QShowEvent *e) override;
    bool event(QEvent *event) override;
    void InitWidgets();
    void InitConnectWidgetsChanged();

public slots:
    void UpdateLabel(const QString &time, const qreal &ratio);
    void UpdateLabelPause(const QString &time, const qreal &ratio);

private slots:
    void SaveValues();
    void ButtonPath_clicked();
    void ButtonPath_alt_clicked();
    void ButtonGenerateText_clicked();
    void SaveButton_status();
    void CheckBoxPrettyFont_Enable();
signals:
    void closedialog();
    void save(const Setting &setting);
    void TimerStatusRequest();

private:
    Setting setting;

    QTabWidget * TabWidget{};
    QLabel *Label_Timer_Prefix{};
    QLabel *Label_Timer{};
    QPushButton *buttonSave{};
    QPushButton *buttonMinimizeToSystemTray{};

    QGroupBox *GroupBox_Tab1{};
    QLabel *Label_UntilBreak{}     ;
    QLabel *Label_BreakDuration{};

    QGroupBox *GroupBox_SystemSetting{};
    QLabel *Label_TrayIconStyle{};
    QComboBox *Combobox_iconsMode{};
    QSpinBox *Spinbox_UntilBreak{};
    QSpinBox *Spinbox_BreakDuration{};

    QGroupBox *GroupBox_Background{};
    QGroupBox *GroupBox_PredeterminedBackground{};
    QGroupBox *GroupBox_Foreground{};

    QLabel *Label_Path{};
    QLabel *label_Path_alt{};
    QLabel *Label_ImageAspectMode{};
    QLineEdit *LineEdit_Path{};
    QLineEdit *LineEdit_Path_alt{};
    QPlainTextEdit *TextEdit_Text{};

    QCheckBox *CheckBox_Spectrum{};
    QCheckBox *CheckBox_Tiling{};
    QCheckBox *CheckBox_Random_circle{};
    QCheckBox *CheckBox_Random_circles{};
    QCheckBox *CheckBox_Neo{};
    QCheckBox *CheckBox_Stripes{};

    QComboBox *Combobox_imageAspectMode{};
    QCheckBox *CheckBox_Clock{};
    QCheckBox *CheckBox_Message{};
    QCheckBox *CheckBox_Logging{};
    QCheckBox *CheckBox_PrettyFont{};
    QCheckBox *CheckBox_Text{};
    QCheckBox *CheckBox_StartupLink{};
    QPushButton *ButtonGenerateText{};

    QWinTaskbarButton *TaskbarButton{};
    QWinTaskbarProgress *TaskbarProgress{};
};

#endif // DIALOG_H

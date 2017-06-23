//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef TRAYICON_H
#define TRAYICON_H

#if defined _WIN32
#define UPDATE_PERIOD_1 40
#else
#define UPDATE_PERIOD_1 40
#endif

#define UPDATE_PERIOD_2 1000

#include <QAction>
#include <QSystemTrayIcon>

#include "view.h"
#include "timer.h"
#include "dialog.h"
#include "aboutwindow.h"
#include "updater.h"


class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    TrayIcon(QWidget *parent = 0);
    ~TrayIcon();

signals:
    void updateLabel(QString text, double ratio);

private slots:



    void ShowView();
    void RefreshmentStart();
    void ViewUpdateTime();
    void DialogUpdateTime();
    void Activated(QSystemTrayIcon::ActivationReason reason);
    void Pause();
    void Quit();
    void About();
    void ShowDialog();
    void CloseDialog();
    void Save(Setting s);

    void TimerStatusSend();
    void slotLanguageChanged(QAction *action);  // this slot is called by the language menu actions

private:
    void ReadSettings();
    void WriteSettings();

    void InitIcons();
    void setCurrentIcon(double ratio);
    void setPauseIcon();
    void setCurrentIconbyCurrentIconRatio();
    void Translate();
    void LoadLanguage(const QString &rLanguage);  // loads a language by the given language shortcur (e.g. de, en)
    void createContextMenu();
    void createLangActionGroup();
    void createActions();


private:
    View *view;
    Dialog *dialog;

    Timer *ViewTimer;
    Timer *DialogTimer;

    QTranslator *Translator;

    QAction *TestAct;
    QAction *ShowSettingAct;
    QAction *QuitAct;
    QAction *PauseAct;
    QAction *AboutAct;
    QActionGroup *LangActGroup;
    QMenu *SubMenuLanguages;
    UpdateAction *UpdaterAct;

    QIcon ipp, i00, i25, i50, i75, i87, i95;

    Setting setting;

    QString LangPath;
    QString LangCurrent;

    bool TrayMessageShowed;
    QString TimeRemains;
    double CurrentIconRatio;

    void CreateOrDeleteAppStartUpLink(bool create);
};

#endif // TRAYICON_H

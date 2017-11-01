//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef TRAYICON_H
#define TRAYICON_H

#if defined _WIN32
#define UPDATE_PERIOD_1 1000
#else
#define UPDATE_PERIOD_1 40
#endif

#define UPDATE_PERIOD_2 1000

#include <QSystemTrayIcon>

#include "view.h"
#include "timer.h"
#include "dialog.h"
#include "updater.h"


class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    TrayIcon(QWidget *parent = 0);
    ~TrayIcon();

signals:
    void updateLabel(const QString &text, const qreal &ratio);
    void updateLabelPause(const QString &text, const qreal &ratio);
    void show_refreshment(QList<QString>, QString, QString, Setting, Timer*);

private slots:
    void ShowView();
    void RefreshmentStart();
    void ViewUpdateTime();
    void DialogUpdateTime();
    void Activated(QSystemTrayIcon::ActivationReason reason);
    void Pause();
    void Skip();
    void Quit();
    void About();
    void ShowDialog();
    void CloseDialog();
    void Save(const Setting &s);

    void LanguageChanged(QAction *action);  // this slot is called by the language menu actions

private:
    void readSettings();
    void writeSettings();
    bool CheckStartupLink();

    void setPauseAct(bool setpause);
    void restartGui();

    void initIcons();
    void setCurrentIcon(qreal ratio);
    void setPauseIcon();
    void setCurrentIconbyCurrentIconRatio();
    void translate();
    void loadLanguage(const QString &rLanguage);  // loads a language by the given language shortcur (e.g. de, en)
    void createContextMenu();
    void createLangActionGroup();
    void createActions();
    void createOrDeleteAppStartUpLink(bool create);

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
    QAction *SkipAct;
    QAction *AboutAct;
    QActionGroup *LangActGroup;
    QMenu *SubMenuLanguages;
    UpdateAction *UpdaterAct;

    QIcon ipp, i00, i02, i04, i06, i08, i10, i12, i14, i15;

    Setting setting;

    QString LangPath;
    QString LangCurrent;

    bool TrayMessageShowed;
    QString TimeRemains;
    qreal CurrentIconRatio;
};

#endif // TRAYICON_H

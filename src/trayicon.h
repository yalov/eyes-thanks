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

#ifndef TRAYICON_H
#define TRAYICON_H

#if defined _WIN32
#define UPDATE_PERIOD_1 40
#else
#define UPDATE_PERIOD_1 40
#endif

#define UPDATE_PERIOD_2 500

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

    void ReadSettings();
    void WriteSettings();

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
    void Save(int pauseinterval, int pausecontinuous, QString Imagespath, QString imageaspectMode,
              bool isLogging, bool istext, bool isclock, bool ismessage30sec, bool isPrettyFont, QString text, IconsMode iconsmode);

    void TimerStatusSend();

private:

    void InitIcons();
    void setCurrentIcon(double ratio);
    void setPauseIcon();
    void setCurrentIconbyCurrentIconRatio();


private:
    View *view;
    Dialog * dialog;

    Timer * ViewTimer;
    Timer * DialogTimer;

    QAction *TestAct;
    QAction *ShowSettingAct;
    QAction *QuitAct;
    QAction *PauseAct;
    QAction *AboutAct;
    UpdateAction *UpdaterAct;

    QIcon ipp,i00, i25, i50, i75, i87, i95;

    int Counter;
    int pauseInterval;
    int pauseContinuous;
    QString ImagesPath;
    QString imageAspectMode;

    bool isLogging;
    bool isText;
    bool isClock;
    bool isMessage30sec;
    bool isPrettyFont;
    QString Text;
    IconsMode iconsMode;


    bool TrayMessageShowed;
    QString TimeRemains;
    double CurrentIconRatio;

};

#endif // TRAYICON_H

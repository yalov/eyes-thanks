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

#ifndef VIEW_H
#define VIEW_H
#include <QtGui>
#include <QtCore>
#include <QCloseEvent>
#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>

#include "graphicstextitemfixbound.h"
#include "global.h"
#include "timer.h"

#define WidgetMode

class View : public QGraphicsView
{
    Q_OBJECT

public:
    enum Methods {
        UNICOLOROUS,
        LINEAR_GRADIENT_DIAGONAL,
        LINEAR_GRADIENT_TEST,
        LINEAR_GRADIENT_RAINBOW,
        LINEAR_GRADIENT_RAINBOW_2,
        LINEAR_GRADIENT_STRIPES,
        LINEAR_GRADIENT_STRIPES_2,
        RAINBOWED_RECTANGLES,
        RANDOM_DOT,
        RANDOM_DOTS,
        RANDOM_DOTS_2,
        NEO,

        COUNT_OF_METHODS
    }; Q_ENUM(Methods)


public:
    View(QWidget *parent = 0);
    ~View();

    void ShowRefreshment(QList<QString> pics_path, QString clock, QString ProgressBarText, Setting _setting);
    void UpdateValues(QString remains_str, double ratio);

signals:
    void view_close();


private:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void SetBackground(double hue_now);
    void SaveSceneToFile(QString dir_path);

    QString picture_path;

    QGraphicsScene *myscene;
    QGraphicsTextItem *clockItem;
    QGraphicsRectItem *ProgressBar;
    QGraphicsRectItem *ProgressBarBound;
    QGraphicsRectItem *ProgressBarBackground;
    GraphicsTextItemFixBound *ProgressBarText;
    QRect ProgressBarRect;
    QGraphicsRectItem *ButtonRectItem;
    Setting setting;

    double Hue_start;
    QGraphicsEllipseItem * Item;
    QElapsedTimer * ElapsedTimerDot;
    int Method;
    bool IsBackgroundUpdate;
    bool RunnedFirstTime;


public:
    GraphicsTextItemFixBound *ButtonText;
};




#endif // VIEW_H

//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
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
#include "testitem.h"

#define WidgetMode

class View : public QGraphicsView
{
    Q_OBJECT

public:
    enum Methods {
        UNICOLOROUS,
        LINEAR_GRADIENT_DIAGONAL,
//        LINEAR_GRADIENT_TEST,
        LINEAR_GRADIENT_RAINBOW,
        LINEAR_GRADIENT_STRIPES,
        LINEAR_GRADIENT_STRIPES_2,
        RAINBOWED_RECTANGLES,
        RANDOM_DOT,
        RANDOM_DOTS,
        RANDOM_DOTS_2,
        OPENGLDOTS,
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
    void opengl_update(double, int);


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
    QGraphicsSimpleTextItem *clockItem;
    QGraphicsRectItem *ProgressBar;
    QGraphicsRectItem *ProgressBarBound;
    QGraphicsRectItem *ProgressBarBackground;
    GraphicsTextItemFixBound *ProgressBarText;
    QRect ProgressBarRect;
    QGraphicsRectItem *ButtonRectItem;
    Setting setting;

    double Hue_start;
    QGraphicsEllipseItem * Item;
    TestItem * refreshment;
    QElapsedTimer * ElapsedTimerDot;
    int Method;
    bool IsBackgroundUpdate;
    bool RunnedFirstTime;
    TestItem * testitem;


public:
    GraphicsTextItemFixBound *ButtonText;
};




#endif // VIEW_H

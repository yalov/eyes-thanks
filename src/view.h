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

#define WidgetMode

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = 0);

    enum Methods {
        UNICOLOROUS,
//        LINEAR_GRADIENT_TEST,
        RAINBOW,
        RAINBOW_STRIPES,
        RAINBOWED_RECTANGLES,
        RANDOM_CIRCLE,
        RANDOM_CIRCLES,
        //OPENGLDOTS,
        NEO,

        COUNT_OF_METHODS
    }; Q_ENUM(Methods)

    void ShowRefreshment(const QList<QString>& pics_path,const QString &clock, const QString &ProgressBarText, const Setting & setting);
    void UpdateValues(const QString &remains_str, const double &ratio);

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
    int SetBackground(double hue_now);
    void SaveSceneToFile(QString dir_path);

    QString picture_path;

    QGraphicsScene *myscene;
    QGraphicsSimpleTextItem *clockItem;
    QGraphicsSimpleTextItem *textItem;
    QGraphicsRectItem *ProgressBar;
    QGraphicsRectItem *ProgressBarBound;
    QGraphicsRectItem *ProgressBarBackground;
    GraphicsTextItemFixBound *ProgressBarText;
    QRect ProgressBarRect;
    QGraphicsRectItem *ButtonRectItem;
    GraphicsTextItemFixBound *ButtonText;

    Setting setting;

    QGraphicsEllipseItem * Item;
    QElapsedTimer * ElapsedTimerDot;
    int Method;
    double Hue_start;
    bool IsBackgroundUpdate;
    bool RunnedFirstTime;
};




#endif // VIEW_H

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
#include <QApplication>
#include <QDesktopWidget>

#include "global.h"
#include "timer.h"
#include "viewitem.h"


class Rect : public QRect {
public:
    Rect(const QRect r): QRect(r) {}
    inline qreal  widthF() const { return qreal(width()); }
    inline qreal heightF() const { return qreal(height()); }
    inline qreal   ratio() const { return widthF() / heightF(); }
};

class TilingItem : public QGraphicsPolygonItem {
public:
    TilingItem(const QPolygonF &hexagon, qreal &hue, int pen_width = 2, QColor pen_color = Qt::black): QGraphicsPolygonItem()
    {
        setPolygon(hexagon);
        setPen(QPen(pen_color, pen_width));

        hue -= floor(hue);
        qreal value = Random(0.4) + 0.20;
        setBrush(QColor::fromHsvF(hue, 1.0, value));
    }
};


class View : public QGraphicsView {
    Q_OBJECT

public:
    explicit View(QWidget *parent = nullptr);

    enum Methods {
        SPECTRUM,
        TILING,
        STRIPES,
        RANDOM_CIRCLE,
        RANDOM_CIRCLES,
        NEO,

        COUNT_OF_METHODS
    }; Q_ENUM(Methods)


    void UpdateValues(const QString &remains_str, const qreal &ratio);

public slots:
    void ShowRefreshment(const QString &clock, const QString &ProgressBarText, const Setting &setting, Timer *viewtimer);

signals:
    void view_close();

private:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void SetPredeterminedBackground();
    QString SetImageBackground(qreal ratio_pic, QPixmap pic);
    void SaveSceneToFile(QString dir_path);

    const Rect default_screen;
    const Rect desktop;

    QString picture_path;

    QGraphicsScene *myscene;
    QGraphicsSimpleTextItem *clockItem;
    QGraphicsSimpleTextItem *textItem;
    ViewItem *ProgressBarItem;
    ViewItem *ButtonItem;

    Setting setting;

    QGraphicsEllipseItem *Item;
    int MethodIndex;
    QList<int> MethodsEnabled;
    bool IsBackgroundUpdate;
    bool RunnedFirstTime;

};

#endif // VIEW_H

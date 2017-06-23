//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef REFRESHMENT_H
#define REFRESHMENT_H
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

#include <QWidget>


#define WidgetMode

class TestItem : public QGraphicsItem
{

public:

    TestItem(QRect screen);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void animate(double hue);
    double hue;
    QRect screen;
    QRectF boundingRect() const override;


};

#endif // REFRESHMENT_H

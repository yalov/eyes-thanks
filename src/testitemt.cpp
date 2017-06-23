//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "testitem.h"
#include <QPainter>

#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>


TestItem::TestItem(QRect _screen) : QGraphicsItem()
{
        screen = _screen;
            qDebug() << "Refreshment";
}

void TestItem::animate(double _hue )
{
    hue = _hue;
    update();
    qDebug() << "animate";

}


void TestItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);


    painter->setBrush(QColor::fromHsvF(hue,1,1,0.5));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(rand()%screen.width(), rand()%screen.height(),
                                    40, 40));

    qDebug() << "paint";
}


QRectF TestItem::boundingRect() const
{
        qDebug() << "boundingRect";
    return QRectF(screen);

}

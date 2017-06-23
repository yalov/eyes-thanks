//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef GRAPHICSTEXTITEMFIXBOUND_H
#define GRAPHICSTEXTITEMFIXBOUND_H

#include <QtGui>
#include <QtCore>
#include <QGraphicsTextItem>

class GraphicsTextItemFixBound : public QGraphicsTextItem
{
private:

    int d1x;
    int d1y;
    int d2x;
    int d2y;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QGraphicsTextItem::paint(painter, option, widget);
        //painter->setPen(Qt::black);
        //painter->drawRect(boundingRect());
    }

public:

    GraphicsTextItemFixBound() : QGraphicsTextItem()
    {
    }

    void setFont(QFont f)
    {

        QGraphicsTextItem::setFont(f);
        d1x =  0;                                            // kde4 0
        d1y = f.pointSize() / 4; // 0;   //f.pointSize()/2;    // kde4 f.pointSize()/4;
        d2x = 0;                                             // kde4 0
        d2y = -f.pointSize() / 4; // 0;    //-f.pointSize()/4; // kde4 -f.pointSize()/4
    }



    void setPos(const QPoint &pos)
    {
        QGraphicsTextItem::setPos(pos.x() - d1x, pos.y() - d1y);
    }

    void setPos(int x, int y)
    {
        QGraphicsTextItem::setPos(QPoint(x - d1x, y - d1y));
    }

    QRectF boundingRect() const
    {
        return QGraphicsTextItem::boundingRect().adjusted(d1x, d1y, d2x, d2y);
    }

};

#endif // GRAPHICSTEXTITEMFIXBOUND_H

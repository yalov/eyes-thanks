#include "viewrandompolygon.h"
#include "global.h"

#include <QBrush>


ViewRandomPolygon::ViewRandomPolygon(QRect desktop)
{
    radius = qMin(desktop.width(), desktop.height()) / 10;
    int dots = Random(3, 7);

    setPolygon(createPolygon(radius, dots));
}


void ViewRandomPolygon::setRandomPos(QRect desktop)
{
    setPos(desktop.topLeft() +
           QPoint(Random(desktop.width() - radius),
                  Random(desktop.height() - radius)));
}

void ViewRandomPolygon::setRamdomColor()
{
    setBrush(QColor::fromHsvF(Random(1.0), 1, 1, 0.5));
}

QPolygonF ViewRandomPolygon::createPolygon(int radius, int dots)
{

    qreal start_angle = Random(2* M_PI/dots);

    QPolygonF p;

    for (int i=0; i < dots; i++)
    {
        auto angle = start_angle + 2* M_PI/dots*i;

        p << QPointF(qSin(angle), qCos(angle))  * radius/2;
    }
    return p;
}


QRectF ViewRandomPolygon::boundingRect() const
{
    return QRectF(-0.5 * radius, -0.5 * radius,
                  radius, radius);
}



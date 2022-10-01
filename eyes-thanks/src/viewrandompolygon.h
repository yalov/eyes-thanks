#ifndef VIEWRANDOMPOLYGON_H
#define VIEWRANDOMPOLYGON_H

#include <QGraphicsItem>

class ViewRandomPolygon : public QGraphicsPolygonItem
{
public:
    ViewRandomPolygon(QRect desktop);
    QRectF boundingRect() const;
    void setRandomPos(QRect desktop);
    void setRamdomColor();

private:
    qreal radius{};
    QPolygonF createPolygon(int radius, int dots);
};

#endif // VIEWRANDOMPOLYGON_H

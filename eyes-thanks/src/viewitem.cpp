#include "viewitem.h"

#include <QFont>
#include <QPen>
#include <QDebug>

ViewItem::ViewItem(bool is_progress_bar, int text_size): IsProgressBar(is_progress_bar)

{
    // main object is rectangle's outline.
    prepareGeometryChange();
    setPen(QPen(Qt::black,1));
    setBrush(Qt::NoBrush);
    setZValue(3);

    Background = new QGraphicsRectItem(this);
    Background->setPen(Qt::NoPen);
    Background->setBrush(Qt::white);
    Background->setOpacity(0.4);
    Background->setZValue(2);

    QFont font;
    font.setPointSize(text_size);
    Text = new QGraphicsSimpleTextItem(this);
    Text->setFont(font);
    Text->setBrush(Qt::black);
    Text->setZValue(4);
    //Text->setCacheMode(QGraphicsItem::ItemCoordinateCache);

    if (IsProgressBar){
        Foreground = new QGraphicsRectItem(this);
        Foreground->setPen(Qt::NoPen);
        Foreground->setBrush(Qt::white);
        Foreground->setOpacity(0.8);
        Foreground->setZValue(2.5);
        //Foreground->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    }
}


void ViewItem::setText(const QString & text){
    prepareGeometryChange();
    Text->setText(text);
}

void ViewItem::setSize(const qreal _w, const qreal _h){
    prepareGeometryChange();
    width = qRound(qMax(Text->boundingRect().width()*1.5, _w));
    height = qRound(qMax(Text->boundingRect().height(), _h));

    setRect(0,0,width,height);
    Background->setRect(0,0, width, height);

    if (IsProgressBar)
        Foreground->setRect(0,0, width, height);

    QPointF text_size (Text->boundingRect().width(),Text->boundingRect().height());
    Text->setPos((QPointF(width,height) - text_size)/2);
}

QRectF ViewItem::boundingRect() const
{
    return QRectF(-0.5 * width, -0.5 * height,
                  width*1.5, height * 1.5);
}

void ViewItem::setProgress(const qreal ratio)
{
    if (IsProgressBar){
        //prepareGeometryChange();
        qreal w = width * (1 - ratio);
        Foreground->setRect(0, 0, w, height);
        //setText(QString("%1 %2 %2 %3").arg(Foreground->rect().x(), 3).arg(Foreground->rect().y(), Foreground->rect().width(),Foreground->rect().height()));
        //Foreground->setBoundingRegionGranularity(1);
    }
}


void ViewItem::setOpacity(const qreal opacity){
    prepareGeometryChange();
    Background->setOpacity(opacity);
}

#include "viewitem.h"

#include <QFont>
#include <QPen>
#include <QDebug>

ViewItem::ViewItem(bool is_progress_bar, int text_size = 16)
    :Text(nullptr), Background(nullptr), Foreground(nullptr)
{
    // main object is rectangle's outline.


    IsProgressBar = is_progress_bar;
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
    Text->setZValue(3);
    Text->setCacheMode(QGraphicsItem::ItemCoordinateCache);

    if (IsProgressBar){
        Foreground = new QGraphicsRectItem(this);
        Foreground->setPen(Qt::NoPen);
        Foreground->setBrush(Qt::white);
        Foreground->setOpacity(0.8);
        Foreground->setZValue(2.5);
        Foreground->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    }
}


void ViewItem::setText(const QString& text){
    Text->setText(text);
}

void ViewItem::setSize(const qreal _w, const qreal _h){

    width = qRound(qMax(Text->boundingRect().width()*1.5, _w));
    height = qRound(qMax(Text->boundingRect().height(), _h));

    setRect(0.5,0.5,width,height);
    Background->setRect(1.0,1.0, width-1, height-1);

    if (IsProgressBar)
        Foreground->setRect(1.0,1.0, width-1, height-1);

    QPointF text_size (Text->boundingRect().width(),Text->boundingRect().height());
    Text->setPos((QPointF(width,height) - text_size)/2);
}


void ViewItem::setProgress(const qreal ratio)
{
    if (IsProgressBar){
        qreal w = 1.0 + (width-1) * (1 - ratio);
        Foreground->setRect(1.0, 1.0, w, height-1);
    }
}


void ViewItem::setOpacity(const qreal opacity){
    Background->setOpacity(opacity);
}

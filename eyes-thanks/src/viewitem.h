#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QGraphicsRectItem>

class ViewItem : public QGraphicsRectItem
{
public:
    ViewItem(bool progressbar, int text_size);
    void setText(const QString &test);
    void setSize(qreal _w = 0, qreal _h = 0);
    void setProgress(const qreal ratio);
    void setOpacity(const qreal opacity);

private:
    QGraphicsSimpleTextItem *Text;
    QGraphicsRectItem *Background;
    QGraphicsRectItem *Foreground;

    bool IsProgressBar;
    qreal width;
    qreal height;
};

#endif // VIEWITEM_H

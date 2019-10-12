#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QGraphicsRectItem>

class ViewItem : public QGraphicsRectItem
{
public:
    ViewItem(bool is_progress_bar, int text_size=16);
    void setText(const QString & text);
    void setSize(qreal _w = 0, qreal _h = 0);
    void setProgress(qreal ratio);
    void setOpacity(qreal opacity);

private:
    QGraphicsSimpleTextItem *Text{};
    QGraphicsRectItem *Background{};
    QGraphicsRectItem *Foreground{};

    bool IsProgressBar;
    qreal width{};
    qreal height{};
};

#endif // VIEWITEM_H

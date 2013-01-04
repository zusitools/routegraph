#ifndef LABEL_H
#define LABEL_H

#include <QGraphicsSimpleTextItem>

class Label : public QGraphicsSimpleTextItem
{
public:
    Label(const QString text, QGraphicsItem *parent = 0);

    QRectF totalBoundingRect();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // LABEL_H

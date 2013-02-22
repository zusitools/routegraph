#ifndef MINWIDTHPATHITEM_H
#define MINWIDTHPATHITEM_H

#include <QGraphicsPathItem>

/**
 * A QGraphicsPathItem that is displayed with a minimum physical width
 * regardless of the zoom level.
 */
class MinWidthPathItem : public QGraphicsPathItem
{
public:
    MinWidthPathItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // MINWIDTHPATHITEM_H

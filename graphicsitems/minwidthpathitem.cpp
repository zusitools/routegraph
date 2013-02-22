#include "minwidthpathitem.h"

#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

MinWidthPathItem::MinWidthPathItem(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsPathItem(parent, scene)
{
}

void MinWidthPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen = this->pen();
    qreal scaleFactor = option->levelOfDetailFromTransform(painter->worldTransform());

    // Always make the pen at least 0.5 physical pixels thick.
    if (scaleFactor >= 0.5) {
        pen.setWidth(1);
    } else {
        pen.setWidth(0.5 / scaleFactor);
    }

    this->setPen(pen);

    QGraphicsPathItem::paint(painter, option, widget);
}

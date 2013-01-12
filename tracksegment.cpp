#include "tracksegment.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

TrackSegment::TrackSegment(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsPathItem(parent, scene)
{
}

void TrackSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen = this->pen();
    qreal scaleFactor = painter->transform().m11();

    qDebug() << scaleFactor;
    // Always make the pen at least 0.5 physical pixels thick.
    if (scaleFactor >= 0.5) {
        pen.setWidth(1);
    } else {
        pen.setWidth(0.5 / scaleFactor);
    }

    this->setPen(pen);

    QGraphicsPathItem::paint(painter, option, widget);
}

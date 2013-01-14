#include "wendepunkt.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPointF>

#include "zvalues.h"

// size *must* be float, else divisions will produce unexpected results
#define SIZE 6.0

Wendepunkt::Wendepunkt(QGraphicsItem *parent, qreal phi) : QGraphicsItem(parent)
{
    this->setRotation(90 - phi);
    this->setZValue(ZVALUE_WENDEPUNKT);
}

QRectF Wendepunkt::boundingRect() const
{
    return QRectF(-SIZE/2 - SIZE/4 - 1, -SIZE - 1, SIZE + 2, SIZE + 2);
}

void Wendepunkt::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(QBrush(Qt::darkMagenta), 1));

    // arrow line -- draw a little bit into curve to reduce artifacts on big zoom levels
    painter->drawLine(-SIZE/2, -0.5, -SIZE/2, -SIZE/2 - 0.1);

    // arrow tip
    painter->drawLine(-SIZE/2, 0, -SIZE/2 - SIZE/4, -SIZE/4);
    painter->drawLine(-SIZE/2, 0, -SIZE/2 + SIZE/4, -SIZE/4);

    // curve
    painter->drawArc(-SIZE/2, -SIZE - 1, SIZE, SIZE, 0, 180 * 16);
}

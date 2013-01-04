#include "viewpoint.h"

#include <cmath>

#include "label.h"

ViewPoint::ViewPoint(const QString name, const float phiZ, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    (new Label(name, this))->setBrush(QBrush(Qt::blue));
    this->phiZ = phiZ;
}

void ViewPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::blue);
    painter->drawLine(0, 0, round(sin(phiZ + 0.5) * VIEWPOINTSIZE), round(cos(phiZ + 0.5) * VIEWPOINTSIZE));
    painter->drawLine(0, 0, round(sin(phiZ - 0.5) * VIEWPOINTSIZE), round(cos(phiZ - 0.5) * VIEWPOINTSIZE));
}

QRectF ViewPoint::boundingRect() const
{
    return QRectF(-VIEWPOINTSIZE, -VIEWPOINTSIZE, VIEWPOINTSIZE, VIEWPOINTSIZE);
}

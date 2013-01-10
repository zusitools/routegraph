#include "startingpoint.h"

StartingPoint::StartingPoint(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text)
    : Marker(parent, pos, phi, text, Qt::darkGreen)
{
}

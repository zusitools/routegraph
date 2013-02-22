#include "graphicsitems/startingpoint.h"

StartingPoint::StartingPoint(QGraphicsItem *parent, qreal phi, const QString text)
    : Marker(parent, phi, text, Qt::darkGreen)
{
}

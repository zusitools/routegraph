#ifndef STARTINGPOINT_H
#define STARTINGPOINT_H

#include "marker.h"

class StartingPoint : public Marker
{
public:
    StartingPoint(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text);
};

#endif // STARTINGPOINT_H

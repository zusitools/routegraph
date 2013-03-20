#ifndef REGISTERMARKER_H
#define REGISTERMARKER_H

#include "graphicsitems/marker.h"

class RegisterMarker : public Marker
{
public:
    RegisterMarker(QGraphicsItem *parent, qreal phi, const QString text);
};

#endif // REGISTERMARKER_H

#ifndef REGISTERMARKER_H
#define REGISTERMARKER_H

#include "graphicsitems/marker.h"
#include "model/register.h"

typedef Register::OccupationState OccupationState;

class RegisterMarker : public Marker
{
    Q_OBJECT
public:
    RegisterMarker(QGraphicsItem *parent, qreal phi, Register *registerToWatch);

public slots:
    void registerOccupationChanged(OccupationState value);
};

#endif // REGISTERMARKER_H

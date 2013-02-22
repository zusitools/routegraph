#ifndef SIGNAL_H
#define SIGNAL_H

#include "marker.h"

class SignalMarker : public Marker
{
public:
    SignalMarker(QGraphicsItem *parent, qreal phi, const QString text);
};

#endif // SIGNAL_H

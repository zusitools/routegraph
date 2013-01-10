#ifndef SIGNAL_H
#define SIGNAL_H

#include "marker.h"

class Signal : public Marker
{
public:
    Signal(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text);
};

#endif // SIGNAL_H

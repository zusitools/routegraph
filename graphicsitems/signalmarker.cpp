#include "graphicsitems/signalmarker.h"

SignalMarker::SignalMarker(QGraphicsItem *parent, qreal phi, const QString text)
    : Marker(parent, phi, text, QColor(255, 35, 0))
{
}

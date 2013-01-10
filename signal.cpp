#include "signal.h"

Signal::Signal(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text)
    : Marker(parent, pos, phi, text, Qt::red)
{
}

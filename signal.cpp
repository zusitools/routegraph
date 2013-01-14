#include "signal.h"

Signal::Signal(QGraphicsItem *parent, qreal phi, const QString text)
    : Marker(parent, phi, text, Qt::red)
{
}

#include "graphicsitems/registermarker.h"

RegisterMarker::RegisterMarker(QGraphicsItem *parent, qreal phi, const QString text)
    : Marker(parent, phi, text, QColor(255, 150, 0))
{
    this->setToolTip(QObject::tr("Register %1").arg(text));
}

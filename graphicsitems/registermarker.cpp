#include "graphicsitems/registermarker.h"

RegisterMarker::RegisterMarker(QGraphicsItem *parent, qreal phi, Register* registerToWatch)
    : Marker(parent, phi, QString::number(registerToWatch->getRegNumber()),
             (registerToWatch->getOccupationState() == Register::Free) ? QColor(255, 150, 0) : QColor(255, 150, 255))
{
    this->setToolTip(QObject::tr("Register %1").arg(QString::number(registerToWatch->getRegNumber())));
    connect(registerToWatch, SIGNAL(occupationChanged(OccupationState)), this, SLOT(registerOccupationChanged(OccupationState)));
}

void RegisterMarker::registerOccupationChanged(Register::OccupationState value)
{
    this->setColor((value == Register::OccupationState::Free) ? QColor(255, 150, 0) : QColor(128, 0, 0));
}

#include "fahrstrassesegment.h"

#include <QDebug>

FahrstrasseSegment::FahrstrasseSegment() : QObject()
{
    m_trackElements = new QList<TrackElement*>();
    m_isOccupied = false;
    m_occupationStartElement = NULL;
}

FahrstrasseSegment::~FahrstrasseSegment()
{
    delete m_trackElements;
}

void FahrstrasseSegment::setIsOccupied(bool isOccupied, TrackElement *occupationStartElement)
{
    bool changed = (isOccupied != m_isOccupied || occupationStartElement != m_occupationStartElement);
    m_isOccupied = isOccupied;
    m_occupationStartElement = (isOccupied && occupationStartElement != firstElement() ? occupationStartElement : NULL);

    if (changed) {
        emit occupationChanged(isOccupied, occupationStartElement);
    }
}

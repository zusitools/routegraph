#ifndef FAHRSTRASSESEGMENT_H
#define FAHRSTRASSESEGMENT_H

#include <QList>
#include <QObject>
#include "model/trackelement.h"

class FahrstrasseSegment : public QObject
{
    Q_OBJECT

public:
    FahrstrasseSegment();
    ~FahrstrasseSegment();

    inline QList<TrackElement*> *trackElements() { return m_trackElements; }
    inline bool isOccupied() { return m_isOccupied; }

    /**
     * Returns the first element of this segment.
     */
    inline TrackElement* firstElement() { return m_trackElements->first(); }

    /**
     * Returns the last element of this segment.
     */
    inline TrackElement* lastElement() { return m_trackElements->last(); }

    /**
     * Returns the element at which the occupation starts. NULL means the whole segment is occupied.
     */
    inline TrackElement* occupationStartElement() { return m_occupationStartElement; }

    /**
     * Sets the occupation of this Fahrstrasse segment. The occupationStartElement
     * specifies at which element the occupation starts (NULL means the whole segment is occupied).
     */
    void setIsOccupied(bool isOccupied, TrackElement *occupationStartElement = NULL);

signals:
    void occupationChanged(bool occupied, TrackElement *occupationStartElement);

private:
    QList<TrackElement*> *m_trackElements;
    bool m_isOccupied;
    TrackElement *m_occupationStartElement;
};

#endif // FAHRSTRASSESEGMENT_H

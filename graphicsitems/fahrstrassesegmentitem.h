#ifndef FAHRSTRASSESEGMENTITEM_H
#define FAHRSTRASSESEGMENTITEM_H

#include <QObject>

#include "graphicsitems/minwidthpathitem.h"
#include "graphicsitems/tracksegment.h"
#include "model/trackelement.h"
#include "model/fahrstrassesegment.h"

class FahrstrasseSegmentItem : public QObject, public MinWidthPathItem
{
    Q_OBJECT

public:
    FahrstrasseSegmentItem(FahrstrasseSegment *segment = 0, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    void setFahrstrasseSegment(FahrstrasseSegment *segment);

    /**
     * Returns the Fahrstrasse segment displayed by this FahrstrasseSegmentItem
     */
    inline FahrstrasseSegment *segment() { return m_segment; }
public slots:
    void segmentOccupationChanged(bool isOccupied, TrackElement *occupationStartElement);

private:
    FahrstrasseSegment *m_segment;
    TrackElement *m_occupationStartElement;

    /**
     * Builds the visible path from the specified start element to the end of the Fahrstrasse segment.
     */
    void buildPath();
};

#endif // FAHRSTRASSESEGMENT_H

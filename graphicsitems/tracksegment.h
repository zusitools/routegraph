#ifndef TRACKSEGMENT_H
#define TRACKSEGMENT_H

#include "graphicsitems/minwidthpathitem.h"

#include "model/trackelement.h"

class TrackSegment : public MinWidthPathItem
{
public:
    TrackSegment(QList<TrackElement *> trackElements, bool showArrows = true, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    inline QGraphicsPathItem *arrows() { return m_arrows; }
private:
    /**
     * The arrows displayed in this track segment.
     */
    QGraphicsPathItem *m_arrows;
};

#endif // TRACKSEGMENT_H

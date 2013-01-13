#ifndef ROUTE_H
#define ROUTE_H

#include <QString>
#include <QList>
#include <QHash>
#include <QGraphicsPathItem>

#include "trackelement.h"
#include "tracksegment.h"
#include "viewpoint.h"
#include "startingpoint.h"
#include "signal.h"

class Route
{
public:
    Route(QString fileName);
    ~Route();

    QList<TrackSegment*> *trackSegments() { return &m_trackSegments; }
    QList<QGraphicsPathItem*> *arrows() { return &m_arrows; }
    QList<ViewPoint*> *viewPoints() { return &m_viewPoints; }
    QList<StartingPoint*> *startingPoints() { return &m_startingPoints; }
    QList<Signal*> *signalList() { return &m_signals; } // Can't use "signals" because it is a reserved Qt keyword

private:
    QHash<int, TrackElement*> trackElements;
    QList<TrackSegment*> m_trackSegments;
    QList<QGraphicsPathItem*> m_arrows;
    QList<Signal*> m_signals;
    QList<ViewPoint*> m_viewPoints;
    QList<StartingPoint*> m_startingPoints;

    /**
     * Returns the track element with the specified number, creating a new
     * entry in the track element hash map if necessary.
     * @param number The number of the track element
     * @return The track element with the specified number.
     */
    TrackElement *getTrackElement(const int number);
};

#endif // ROUTE_H

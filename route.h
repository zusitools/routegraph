#ifndef ROUTE_H
#define ROUTE_H

#include <QString>
#include <QList>
#include <QHash>
#include <QGraphicsPathItem>

#include "trackelement.h"
#include "viewpoint.h"
#include "startingpoint.h"
#include "signal.h"

class Route
{
public:
    Route(QString fileName);

    QList<QGraphicsPathItem*> *trackSegments() { return &m_trackSegments; }
    QList<ViewPoint*> *viewPoints() { return &m_viewPoints; }
    QList<StartingPoint*> *startingPoints() { return &m_startingPoints; }
    QList<Signal*> *signalList() { return &m_signals; } // Can't use "signals" because it is a reserved Qt keyword

private:
    QHash<int, TrackElement*> trackElements;
    QList<QGraphicsPathItem*> m_trackSegments;
    QList<Signal*> m_signals;
    QList<ViewPoint*> m_viewPoints;
    QList<StartingPoint*> m_startingPoints;
    TrackElement *getTrackElement(const int number);
};

#endif // ROUTE_H

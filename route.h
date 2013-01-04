#ifndef ROUTE_H
#define ROUTE_H

#include <QString>
#include <QList>
#include <QHash>
#include <QGraphicsPathItem>

#include "trackelement.h"
#include "viewpoint.h"

class Route
{
public:
    Route(QString fileName);

    QList<QGraphicsPathItem*> *trackSegments() { return &m_trackSegments; }
    QList<ViewPoint*> *viewPoints() { return &m_viewPoints; }

private:
    QHash<int, TrackElement*> trackElements;
    QList<QGraphicsPathItem*> m_trackSegments;
    QList<ViewPoint*> m_viewPoints;
    TrackElement *getTrackElement(const int number);
};

#endif // ROUTE_H

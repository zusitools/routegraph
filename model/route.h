#ifndef ROUTE_H
#define ROUTE_H

#include <QList>
#include <QHash>
#include <QGraphicsPathItem>

#include "model/trackelement.h"
#include "graphicsitems/tracksegment.h"
#include "model/fahrstrassesegment.h"
#include "model/station.h"
#include "model/timetableentry.h"
#include "graphicsitems/viewpoint.h"
#include "graphicsitems/startingpoint.h"
#include "graphicsitems/wendepunkt.h"
#include "graphicsitems/signalmarker.h"

class Route
{
public:
    Route(QString fileName);
    ~Route();

    inline QList<TrackElement*> trackElements() { return m_trackElements.values(); }
    QList<ViewPoint*> *viewPoints() { return &m_viewPoints; }
    QList<StartingPoint*> *startingPoints() { return &m_startingPoints; }
    QList<Wendepunkt*> *wendepunkte() { return &m_wendepunkte; }
    QList<FahrstrasseSegment*> *fahrstrasseSegments() { return &m_fahrstrasseSegments; }

    /**
     * Returns a list of this route's stations.
     */
    inline QList<Station*> *stations() { return &m_stations; }

    /**
     * Returns the path to the .ls file as specified in the .str file.
     */
    inline QString lsFile() { return m_lsFile; }


    /**
     * Returns a list of all track elements that have an associated signal.
     */
    QList<TrackElement*> trackElementsWithSignals();

    /**
     * Returns the track element with the specified number or NULL if the specified track element does not exist.
     */
    TrackElement *trackElement(const int number);

    /**
     * Finds all possible routes (consisting of Fahrstrasse segments) from the element with the given number
     * to the signals defined by the given timetable entry.
     * At most "recursion_depth" track elements with signals will be encountered before canceling the search.
     */
    QList<QList<FahrstrasseSegment *> *> findRoutesTo(uint32_t startElementNumber, TimetableEntry *target, int recursionDepth);

private:
    QHash<int, TrackElement*> m_trackElements;
    QList<ViewPoint*> m_viewPoints;
    QList<StartingPoint*> m_startingPoints;
    QList<Wendepunkt*> m_wendepunkte;
    QList<FahrstrasseSegment*> m_fahrstrasseSegments;
    QList<Station*> m_stations;

    QString m_lsFile;

    /**
     * Track elements by their (integer) start X coordinate. Is only needed during the constructor
     * and deleted afterwards.
     */
    QHash<int, QList<TrackElement*>*> trackElementsByStartX;

    /**
     * Returns the track element with the specified number, creating a new
     * entry in the track element hash map if necessary.
     * @param number The number of the track element
     * @return The track element with the specified number.
     */
    TrackElement *getTrackElement(const int number);

    /**
     * Recursively finds a route to the given timetable entry.
     */
    void findRouteRec(QList<QList<FahrstrasseSegment *> *> &results, QList<FahrstrasseSegment *> &currentPath, int recursionDepth, TimetableEntry &target);

    /**
     * Sets this track element and all its successors to "reachable from starting point".
     */
    void setReachableRec(TrackElement *const trackElement) const;

    /**
     * Returns the opposite element of the given track element, if it exists, or NULL otherwise.
     */
    TrackElement *findOppositeTrackElement(TrackElement *te) const;
};

#endif // ROUTE_H

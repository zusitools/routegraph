#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QList>

#include "model/route.h"
#include "graphicsitems/label.h"
#include "graphicsitems/signalmarker.h"
#include "graphicsitems/tracksegment.h"
#include "graphicsitems/fahrstrassesegmentitem.h"
#include "graphicsitems/fahrstrasseaufloesenmarker.h"

class TrackView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TrackView(QWidget *parent = 0);
    ~TrackView();

    void scaleBy(double factor);
    void wheelEvent(QWheelEvent *event);
    void zoomIn();
    void zoomOut();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void setRoute(Route *route);

    /**
     * Sets the visibility of the signal names.
     */
    void setSignalNamesVisible(bool visible);

    /**
     * Sets the visibility of the starting point names.
     */
    void setStartingPointNamesVisible(bool visible);

    /**
     * Sets the visibility of the view point names.
     */
    void setViewPointNamesVisible(bool visible);

    /**
     * Sets the visibility of the station names.
     */
    void setStationNamesVisible(bool visible);

    /**
     * Sets whether the text scales when zooming
     */
    void setTextScaling(bool on);

    /**
     * Returns the Fahrstrasse segment items of this track view.
     */
    inline QList<FahrstrasseSegmentItem*> *fahrstrasseSegmentItems() { return &m_fahrstrasseSegmentItems; }

    /**
     * Clears the scene and all associated information.
     */
    void clearRoute();

    /**
     * Sets the text scaling for the given label to on or off.
     */
    static void triggerTextScale(Label *label, bool on);
signals:
    void mouseDoubleClicked(QMouseEvent *event);

public slots:

private:
    Route *m_route;
    QList<SignalMarker*> m_signals;
    QList<TrackSegment*> m_trackSegments;
    QList<FahrstrasseSegmentItem*> m_fahrstrasseSegmentItems;
    QList<Label*> m_stationLabels;
    QList<FahrstrasseAufloesenMarker*> m_fahrstrasseAufloesenMarkers;

    /**
     * Creates the track segments from m_route's track elements.
     */
    void createTrackSegments();

    /**
     * Indicates whether the right mouse button is pressed
     */
    bool m_rmbPressed;

    /**
     * The base point for rotation using the right mouse button
     */
    QPoint m_dragStartPoint;
};

#endif // TRACKVIEW_H

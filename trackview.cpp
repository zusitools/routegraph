#include "trackview.h"

#include <cmath>
#include <QDebug>

#include "graphicsitems/signalmarker.h"
#include "model/fahrstrassesegment.h"
#include "model/station.h"

// Uncomment this to give each track segment a random color
// #define RANDOM_COLORS

TrackView::TrackView(QWidget *parent) : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    m_rmbPressed = false;
    m_route = NULL;
}

TrackView::~TrackView()
{
    qDeleteAll(m_signals);
    qDeleteAll(m_trackSegments);
}

void TrackView::scaleBy(double factor)
{
    scale(factor, factor);
}

void TrackView::wheelEvent(QWheelEvent *event)
{
   if (event->modifiers() == Qt::ControlModifier)
    {
       setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
       scaleBy(std::pow(4.0 / 3.0, (event->delta() / 240.0)));
       setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void TrackView::zoomIn()
{
    scaleBy(1.1);
}

void TrackView::zoomOut()
{
    scaleBy(1.0 / 1.1);
}

void TrackView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::RightButton)) {
        m_rmbPressed = true;
        m_dragStartPoint = event->pos();
    }

    QGraphicsView::mousePressEvent(event);
}

void TrackView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_rmbPressed) {
        this->rotate((event->pos() - m_dragStartPoint).y());
        m_dragStartPoint = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void TrackView::mouseReleaseEvent(QMouseEvent *event)
{
    m_rmbPressed = false;
    QGraphicsView::mouseReleaseEvent(event);
}

void TrackView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        resetTransform();
        fitInView(this->sceneRect(), Qt::KeepAspectRatio);
    } else {
        emit mouseDoubleClicked(event);
    }
}

void TrackView::clearRoute() {
    if (this->scene()) {
        this->setScene(NULL);
        // This deletes all the items in the scene, too.
        delete this->scene();
    }

    m_signals.clear();
    m_trackSegments.clear();
    m_fahrstrasseSegmentItems.clear();
    m_stationLabels.clear();
    m_fahrstrasseAufloesenMarkers.clear();
    m_manualRegisterMarkers.clear();
    m_automaticRegisterMarkers.clear();
}

void TrackView::setRoute(Route *route)
{
    this->m_route = route;
    this->createTrackSegments();

    QGraphicsScene* scene = new QGraphicsScene(this);

    // disable indexing because of animations
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    foreach (TrackSegment* te, m_trackSegments) {
        scene->addItem(te);
        scene->addItem(te->arrows());
    }

    foreach (ViewPoint* vp, *(m_route->viewPoints())) {
        scene->addItem(vp);
    }

    foreach (TrackElement* te, m_route->trackElements()) {
        // Signal
        if (te->hasSignal()) {
            SignalMarker *sm = new SignalMarker(0, te->line().angle(), te->stationName() + ", " + te->trackName());
            sm->setPos(te->line().p2());
            scene->addItem(sm);
            m_signals.append(sm);
        }

        // "Fahrstrasse auflösen" markers
        if (te->ereignis() == 3002) {
            FahrstrasseAufloesenMarker *fm = new FahrstrasseAufloesenMarker();
            fm->setPos(te->line().p2());
            fm->setRotation(270 - te->line().angle());
            scene->addItem(fm);
            m_fahrstrasseAufloesenMarkers.append(fm);
        }

        // Register markers
        if (te->registerNo() != 0) {
            RegisterMarker *m = new RegisterMarker(0, te->line().angle(), QString::number(te->registerNo()));
            m->setPos(te->line().pointAt(0.5));
            scene->addItem(m);

            if (te->registerNo() < 1000) {
                m_manualRegisterMarkers.append(m);
            } else {
                m_automaticRegisterMarkers.append(m);
            }
        }
    }

    foreach (Wendepunkt* wp, *(m_route->wendepunkte())) {
        scene->addItem(wp);
    }

    foreach (Station* st, *(m_route->stations())) {
        Label *stationLabel = new Label(st->name);
        stationLabel->setPos(st->pos);
        stationLabel->setPen(QPen(Qt::darkRed));
        scene->addItem(stationLabel);
        m_stationLabels.append(stationLabel);
    }

    foreach (StartingPoint* sp, *(m_route->startingPoints())) {
        scene->addItem(sp);
    }

    foreach (FahrstrasseSegment *fs, *m_route->fahrstrasseSegments()) {
        FahrstrasseSegmentItem *newItem = new FahrstrasseSegmentItem(fs);
        m_fahrstrasseSegmentItems.append(newItem);
        scene->addItem(newItem);
    }

    this->resetTransform();
    this->setScene(scene);
}

void TrackView::setSignalNamesVisible(bool visible)
{
    foreach (SignalMarker *sm, m_signals) {
        sm->label()->setVisible(visible);
    }
}

void TrackView::setStartingPointNamesVisible(bool visible)
{
    if (m_route) {
        foreach (StartingPoint* sp, *(m_route->startingPoints())) {
            sp->label()->setVisible(visible);
        }
    }
}

void TrackView::setViewPointNamesVisible(bool visible)
{
    if (m_route) {
        foreach (ViewPoint* vp, *(m_route->viewPoints())) {
            vp->label()->setVisible(visible);
        }
    }
}

void TrackView::setStationNamesVisible(bool visible)
{
    foreach (Label *label, m_stationLabels) {
        label->setVisible(visible);
    }
}

void TrackView::setAutomaticRegistersVisible(bool visible)
{
    foreach (RegisterMarker *rm, m_automaticRegisterMarkers) {
        rm->setVisible(visible);
    }
}

void TrackView::setManualRegistersVisible(bool visible)
{
    foreach (RegisterMarker *rm, m_manualRegisterMarkers) {
        rm->setVisible(visible);
    }
}

void TrackView::setTextScaling(bool on)
{
    foreach (Label *label, m_stationLabels) {
        triggerTextScale(label, on);
    }

    foreach (SignalMarker* sig, m_signals) {
        triggerTextScale(sig->label(), on);
    }

    if (!m_route) {
        return;
    }

    foreach (ViewPoint* vp, *(m_route->viewPoints())) {
        triggerTextScale(vp->label(), on);
    }

    foreach (StartingPoint* sp, *(m_route->startingPoints())) {
        triggerTextScale(sp->label(), on);
    }
}

void TrackView::setUnreachableTrackSegmentsVisible(bool on)
{
    foreach (TrackSegment *ts, m_unreachableTrackSegments) {
        ts->setVisible(on);
    }
}

void TrackView::createTrackSegments() {
    foreach (TrackElement *te, m_route->trackElements()) {
        if (!te->isStartingPointOfSegment()) { continue; }

        QList<TrackElement*> elements; // The elements out of which to form the track segment
        elements.append(te);

        bool reachable = te->isReachableFromStartingPoint();

        while (te->next.size() > 0 && te->next.front()->prev.front() == te && !te->next.front()->isStartingPointOfSegment()) {
            te = te->next.front();
            elements.append(te);
        }

        TrackSegment *trackSegment = new TrackSegment(elements, !te->tunnel()); // do not show arrows in tunnels, it looks ugly
        m_trackSegments.append(trackSegment);
        if (!reachable) {
            m_unreachableTrackSegments.append(trackSegment);
        }

#ifdef RANDOM_COLORS
        QColor penColor = QColor(qrand() % 256, qrand() % 256, qrand() % 256);
# else
        QColor penColor = te->isStartingSegment() ? (te->electrified() ? Qt::darkGreen : Qt::green) : (te->electrified() ? Qt::black : Qt::darkGray);
# endif

        trackSegment->setPen(QPen(
                             QBrush(penColor),
                             te->bothDirections() ? 2 : 1,
                             te->tunnel() ? Qt::DotLine : Qt::SolidLine,
                             Qt::FlatCap));

        trackSegment->arrows()->setPen(QPen(QBrush(penColor), 1, Qt::SolidLine, Qt::FlatCap));
    }

    qDebug() << m_trackSegments.count() << "track segments";
}

void TrackView::triggerTextScale(Label *label, bool on) {
    if (label) {
        label->setFlag(QGraphicsItem::ItemIgnoresTransformations, !on);
        label->font().setPointSize(on ? 30 : 8);
    }
}

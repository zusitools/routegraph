#include "graphicsitems/tracksegment.h"

#include "zvalues.h"
#include <qmath.h>

#define ARROWEVERY 100
#define ARROWLENGTH 10.0
#define ARROWANGLE asin(0.95 / ARROWLENGTH)

TrackSegment::TrackSegment(QList<TrackElement*> trackElements, bool showArrows, QGraphicsItem *parent, QGraphicsScene *scene) : MinWidthPathItem(parent, scene)
{
    // Construct two path items: one for the track segment and one for the segment’s direction arrows,
    // the latter positioned below. This is to prevent overlap on routes which can be used in both direction.
    // The arrows are not shown for tunnel segments because it looks ugly.
    QPainterPath path;
    QPainterPath arrowPath;

    // Don't treat the arrows item as a child item because we want the arrows to have a
    // different Z value.
    m_arrows = new QGraphicsPathItem();

    this->setZValue(ZVALUE_TRACK);
    m_arrows->setZValue(ZVALUE_ARROWS);

    if (trackElements.count() == 0) {
        return;
    }

    path.moveTo(trackElements.first()->shiftedLine().p1());

    foreach (TrackElement *te, trackElements) {
        path.lineTo(te->shiftedLine().p2());
    }

    this->setPath(path);

    if (showArrows) {
        // Draw little arrows to indicate the direction of the track element.
        // The arrows are drawn every ARROWEVERY meters, but at least once per track segment.
        // They also are drawn at least ARROWLENGTH m from the beginning of the track segment.
        qreal pathLength = path.length();
        int numArrows = ((pathLength - ARROWLENGTH) / ARROWEVERY) + 1;
        double firstArrowPos = ARROWLENGTH + (pathLength - ARROWLENGTH - ARROWEVERY * (numArrows - 1)) / 2;

        // Store arrow base points in a map beforehand because drawing them one after another could
        // influence the computation of the following points.
        // TODO this does not apply any more, simplify the code.
        // We have to store pointers to the points in the map because QPointF does not implement operator<.
        QMap<QPointF*, qreal> arrowBasePoints;
        for (int i = 0; i < numArrows; i++) {

            // Take the maximum of the angles at the arrow base and arrow end to prevent arrows from
            // pointing outside the opposite track segment in curves.
            // TODO really draw them in curves
            qreal percentArrowBase = std::min(1.0, (firstArrowPos + ARROWEVERY * i) / pathLength);
            qreal percentArrowEnd = std::max(0.0, (firstArrowPos + ARROWEVERY * i - ARROWLENGTH) / pathLength);

            arrowBasePoints.insert(new QPointF(path.pointAtPercent(percentArrowBase)),
                                   std::max(path.angleAtPercent(percentArrowBase), path.angleAtPercent(percentArrowEnd)) / 180 * M_PI);
        }

        foreach (QPointF* arrowBasePoint, arrowBasePoints.keys()) {
            qreal angle = M_PI - arrowBasePoints[arrowBasePoint] + ARROWANGLE;

            arrowPath.moveTo(*arrowBasePoint);
            arrowPath.lineTo(*arrowBasePoint + QPointF(cos(angle) * ARROWLENGTH, sin(angle) * ARROWLENGTH));
        }

        qDeleteAll(arrowBasePoints.keys());
        m_arrows->setPath(arrowPath);
    }
}

void TrackSegment::setVisible(bool visible)
{
    m_arrows->setVisible(visible);
    MinWidthPathItem::setVisible(visible);
}

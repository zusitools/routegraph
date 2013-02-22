#include "fahrstrassesegmentitem.h"

#include <QPen>
#include "zvalues.h"

FahrstrasseSegmentItem::FahrstrasseSegmentItem(FahrstrasseSegment *segment, QGraphicsItem *parent, QGraphicsScene *scene)
    : QObject(), MinWidthPathItem(parent, scene)
{
    this->setPen(QPen(QColor(255, 187, 0)));
    this->setZValue(ZVALUE_FAHRSTRASSESEGMENTS);
    this->m_segment = NULL;
    this->m_occupationStartElement = NULL;

    if (segment) {
        this->setFahrstrasseSegment(segment);
        this->setVisible(segment->isOccupied());
    } else {
        this->setVisible(false);
    }
}

void FahrstrasseSegmentItem::setFahrstrasseSegment(FahrstrasseSegment *segment)
{
    if (m_segment) {
        disconnect(m_segment);
    }

    this->m_segment = segment;
    this->m_occupationStartElement = NULL;
    buildPath();
    connect(m_segment, SIGNAL(occupationChanged(bool, TrackElement*)), this, SLOT(segmentOccupationChanged(bool, TrackElement*)));
}

void FahrstrasseSegmentItem::buildPath() {
    QPainterPath path;

    if (m_segment->trackElements()->count() > 0) {
        int i = 0;
        while (m_occupationStartElement != NULL && i < m_segment->trackElements()->count() && m_segment->trackElements()->at(i) != m_occupationStartElement) {
            i++;
        }

        if (i < m_segment->trackElements()->count()) {
            path.moveTo(m_segment->trackElements()->at(i)->shiftedLine(1.44).p1());

            for (int j = i; j < m_segment->trackElements()->count(); j++) {
                path.lineTo(m_segment->trackElements()->at(j)->shiftedLine(1.44).p2());
            }
        }
    }

    this->setPath(path);
}

void FahrstrasseSegmentItem::segmentOccupationChanged(bool isOccupied, TrackElement *occupationStartElement)
{
    if (isOccupied && occupationStartElement != m_occupationStartElement) {
        m_occupationStartElement = occupationStartElement;
        buildPath();
    }

    this->setVisible(isOccupied);
}

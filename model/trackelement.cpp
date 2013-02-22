#include "model/trackelement.h"

#include <QPainterPath>

#include <cmath>

TrackElement::TrackElement(const int number)
{
    this->m_number = number;
    this->m_bothDirections = false;
    this->m_isStartingPoint = false;
    this->m_isStartingSegment = false;
    this->m_hasSignal = false;
    this->m_ereignis = 0;
}

TrackElement::~TrackElement()
{
    prev.clear();
    next.clear();
}

bool TrackElement::isOppositeOf(TrackElement *te)
{
    double deltaX1 = fabs(this->line().p2().x() - te->line().p1().x());
    double deltaX2 = fabs(this->line().p1().x() - te->line().p2().x());
    double deltaY1 = fabs(this->line().p2().y() - te->line().p1().y());
    double deltaY2 = fabs(this->line().p1().y() - te->line().p2().y());

    return (deltaX1 < 0.0001) && (deltaY1 < 0.0001) && (deltaX2 < 0.0001) && (deltaY2 < 0.0001) ;
}

bool TrackElement::isStartingPointOfSegment()
{
    // An element without a predecessor is trivially a starting point of a segment
    if (prev.size() == 0) {
        return true;
    }

    // An element with predecessor(s) is the starting point of a segment if
    //  - it is not the first successor of its predecessor or
    //  - it differs from its predecessors in a property (electrified, bothDirections, tunnel)
    // If there are multiple predecessors, the first predecessor is taken.
    TrackElement *pred = prev.front();
    return (pred->next.front() != this) || (pred->electrified() ^ this->m_electrified) || (pred->bothDirections() ^ this->m_bothDirections) || (pred->tunnel() ^ this->m_tunnel)  || (pred->isStartingSegment() ^ this->m_isStartingSegment);
}

bool TrackElement::isStartingPointOfFahrstrasseSegment()
{
    if (prev.size() != 1 || this->m_isStartingPoint) {
        return true;
    }

    TrackElement *pred = prev.front();
    return pred->next.size() != 1 || pred->hasSignal() || pred->ereignis() == 3002;
}

void TrackElement::deleteFromNeighbors()
{
    std::vector<TrackElement*>::iterator iter1;
    std::vector<TrackElement*>::iterator iter2;

    // Remove this element as a predecessor of all successor elements
    for (iter1 = next.begin(); iter1 != next.end(); ++iter1) {
        TrackElement *successor = *iter1;
        for (iter2 = successor->prev.begin(); iter2 != successor->prev.end(); ++iter2) {
            if ((*iter2) == this) {
                successor->prev.erase(iter2);
                break;
            }
        }
    }

    // Remove this element as a successor of all successor elements
    for (iter1 = prev.begin(); iter1 != prev.end(); ++iter1) {
        TrackElement *predecessor = *iter1;
        for (iter2 = predecessor->next.begin(); iter2 != predecessor->next.end(); ++iter2) {
            if ((*iter2) == this) {
                predecessor->next.erase(iter2);
                break;
            }
        }
    }
}

QLineF TrackElement::shiftedLine(qreal shiftAmount)
{
    if (!this->m_bothDirections) {
        // Shift by a little less than one line width to prevent drawing artifacts for opposite segments
        double angle = (this->line().angle() / 180 * M_PI); // angle in radian
        return this->line().translated(QPointF(sin(angle) * shiftAmount, cos(angle) * shiftAmount));
    } else {
        return this->m_line;
    }
}

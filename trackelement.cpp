#include "trackelement.h"

#include <QPainterPath>

#include <cmath>

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
    return (pred->next.front() != this) || (pred->electrified() ^ this->m_electrified) || (pred->bothDirections() ^ this->m_bothDirections) || (pred->tunnel() ^ this->m_tunnel)  || (pred->isStartingPoint() ^ this->m_isStartingPoint);
}

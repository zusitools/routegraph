#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

#include <QString>
#include <QLineF>
#include <vector>

class TrackElement
{
public:
    TrackElement(const int number);
    ~TrackElement();

    // The successor elements of this element
    std::vector<TrackElement*> next;

    // The predecessor elements of this element
    std::vector<TrackElement*> prev;

    // Returns whether this track element and te point in the opposite direction
    bool isOppositeOf(TrackElement *te);

    // Returns whether this track element should start a new track segment
    bool isStartingPointOfSegment();

    // Deletes a track element from its predecessor and successor elements
    void deleteFromNeighbors();

    // Returns this element’s line shifted half a line width to the right if this element is a single direction element
    QLineF shiftedLine();

    QLineF line() {  return m_line; }
    int number() { return m_number; }
    bool electrified() { return m_electrified; }
    bool tunnel() { return m_tunnel; }
    bool bothDirections() { return m_bothDirections; }
    bool isStartingPoint() { return m_isStartingPoint; }
    bool hasSignal() { return m_hasSignal; }

    void setLine(const QLineF line) { m_line = line; }
    void setElectrified(const bool electrified) { m_electrified = electrified; }
    void setTunnel(const bool tunnel) { m_tunnel = tunnel; }
    void setBothDirections(const bool bothDirections) { m_bothDirections = bothDirections; }
    void setIsStartingPoint(const bool isStartingPoint) { m_isStartingPoint = isStartingPoint; }
    void setHasSignal(const bool hasSignal) { m_hasSignal = hasSignal; }

private:
    QLineF m_line;
    int m_number;
    bool m_electrified;
    bool m_tunnel;
    bool m_bothDirections;
    bool m_isStartingPoint;
    bool m_hasSignal;
};

#endif // TRACKELEMENT_H

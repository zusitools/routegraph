#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

#include <QString>
#include <QLineF>

class TrackElement
{
public:
    TrackElement(const int number) {
        this->m_number = number;
        this->m_isStartingPoint = false;
        this->m_hasSignal = false;
    }

    // The successor elements of this element
    std::vector<TrackElement*> next; // = std::vector<TrackElement*>(3);

    // The predecessor elements of this element
    std::vector<TrackElement*> prev;

    bool isOppositeOf(TrackElement *te);
    bool isStartingPointOfSegment();

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

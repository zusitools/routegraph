#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

#include <QString>
#include <QLineF>
#include <vector>
#include <stdint.h>

class FahrstrasseSegment;

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

    // Returns whether this track element should start a new Fahrstraße segment
    bool isStartingPointOfFahrstrasseSegment();

    // Deletes a track element from its predecessor and successor elements
    void deleteFromNeighbors();

    // Returns this element’s line shifted to the right if this element is a single direction element
    QLineF shiftedLine(qreal shiftAmount = 0.95 / 2);

    QLineF line() {  return m_line; }
    uint32_t number() { return m_number; }
    bool electrified() { return m_electrified; }
    bool tunnel() { return m_tunnel; }
    bool bothDirections() { return m_bothDirections; }
    bool isStartingPoint() { return m_isStartingPoint; }
    inline bool isStartingSegment() { return m_isStartingSegment; }
    bool hasSignal() { return m_hasSignal; }

    /**
     * The name of the station associated with this element's signal (if present).
     */
    inline QString stationName() const { return m_stationName; }

    /**
     * The name of the station associated with this element's signal (if present).
     */
    inline QString trackName() const { return m_trackName; }

    /**
     * Returns a pointer to this element's Fahrstraße segment
     */
    inline FahrstrasseSegment *fahrstrasseSegment() const { return m_fahrstrasseSegment; }

    /**
     * Returns the Ereignis number of this element.
     */
    inline unsigned short ereignis() { return m_ereignis; }

    /**
     * Returns the number of this element's register.
     */
    inline unsigned int registerNo() { return m_registerNo; }

    void setLine(const QLineF line) { m_line = line; }
    void setElectrified(const bool electrified) { m_electrified = electrified; }
    void setTunnel(const bool tunnel) { m_tunnel = tunnel; }
    void setBothDirections(const bool bothDirections) { m_bothDirections = bothDirections; }
    void setIsStartingPoint(const bool isStartingPoint) { m_isStartingPoint = isStartingPoint; }
    inline void setIsStartingSegment(const bool isStartingSegment) { m_isStartingSegment = isStartingSegment; }
    void setHasSignal(const bool hasSignal) { m_hasSignal = hasSignal; }
    inline void setEreignis(const unsigned short ereignis) { m_ereignis = ereignis; }
    inline void setFahrstrasseSegment(FahrstrasseSegment* fahrstrasseSegment) { m_fahrstrasseSegment = fahrstrasseSegment; }
    inline void setRegisterNo(const unsigned int registerNo) { m_registerNo = registerNo; }
    inline void setStationName(const QString stationName) { m_stationName = stationName; }
    inline void setTrackName(const QString trackName) { m_trackName = trackName; }

private:
    QLineF m_line;
    int m_number;
    bool m_electrified;
    bool m_tunnel;
    bool m_bothDirections;
    bool m_isStartingPoint;
    bool m_isStartingSegment;
    bool m_hasSignal;
    unsigned short m_ereignis;
    FahrstrasseSegment *m_fahrstrasseSegment;
    unsigned int m_registerNo;

    QString m_stationName;
    QString m_trackName;
};

#endif // TRACKELEMENT_H

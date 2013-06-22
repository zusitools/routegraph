#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

#include <QString>
#include <QLineF>
#include <vector>
#include <stdint.h>

QT_FORWARD_DECLARE_CLASS(FahrstrasseSegment)
QT_FORWARD_DECLARE_CLASS(Ereignis)
QT_FORWARD_DECLARE_CLASS(TrackElementSignal)

struct TrackElementDirectionInfo {
    TrackElementSignal *signal;
    std::vector<Ereignis*> ereignisse;
    uint32_t registerNo;

    bool hasEreignis(uint32_t ereignisNo);
};

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
    bool bothDirections() { return m_directionInfoNormal != NULL && m_directionInfoOpposite != NULL; }
    bool isStartingPoint() { return m_isStartingPoint; }
    inline char zusiVersion() const { return m_zusiVersion; }
    inline bool isStartingSegment() { return m_isStartingSegment; }
    inline TrackElementDirectionInfo *directionInfo(bool dirNormal) const { return dirNormal ? m_directionInfoNormal : m_directionInfoOpposite; }
    inline void addDirectionInfo(bool dirNormal) { if (dirNormal) m_directionInfoNormal = new TrackElementDirectionInfo(); else m_directionInfoOpposite = new TrackElementDirectionInfo(); }

    /**
     * Returns a pointer to this element's Fahrstraße segment
     */
    inline FahrstrasseSegment *fahrstrasseSegment() const { return m_fahrstrasseSegment; }

    /**
     * Returns whether this track element is reachable from any starting point of the route.
     */
    inline bool isReachableFromStartingPoint() const { return m_isReachableFromStartingPoint; }

    void setLine(const QLineF line) { m_line = line; }
    void setElectrified(const bool electrified) { m_electrified = electrified; }
    void setTunnel(const bool tunnel) { m_tunnel = tunnel; }
    void setIsStartingPoint(const bool isStartingPoint) { m_isStartingPoint = isStartingPoint; }
    inline void setIsStartingSegment(const bool isStartingSegment) { m_isStartingSegment = isStartingSegment; }
    inline void setZusiVersion(const char value) { m_zusiVersion = value; }
    inline void setFahrstrasseSegment(FahrstrasseSegment* fahrstrasseSegment) { m_fahrstrasseSegment = fahrstrasseSegment; }
    inline void setRegisterNo(const unsigned int registerNo) { m_registerNo = registerNo; }
    inline void setIsReachableFromStartingPoint(const bool isReachable) { m_isReachableFromStartingPoint = isReachable; }

private:
    QLineF m_line;
    int m_number;
    bool m_electrified;
    bool m_tunnel;
    bool m_isStartingPoint;
    bool m_isStartingSegment;
    char m_zusiVersion;
    FahrstrasseSegment *m_fahrstrasseSegment;
    unsigned int m_registerNo;
    bool m_isReachableFromStartingPoint;

    TrackElementDirectionInfo *m_directionInfoNormal;
    TrackElementDirectionInfo *m_directionInfoOpposite;
};

#endif // TRACKELEMENT_H

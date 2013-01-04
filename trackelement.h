#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

#include <QString>
#include <QLineF>

#include "signal.h"
#include "label.h"

class TrackElement
{
public:
    TrackElement(const int number) { this->m_number = number; }

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
    Signal *signal() { return m_signal; }

    void setLine(const QLineF line) { m_line = line; }
    void setElectrified(const bool electrified) { m_electrified = electrified; }
    void setTunnel(const bool tunnel) { m_tunnel = tunnel; }
    void setBothDirections(const bool bothDirections) { m_bothDirections = bothDirections; }
    void setSignal(Signal *signal) { m_signal = signal; }

private:
    QLineF m_line;
    int m_number;
    bool m_electrified;
    bool m_tunnel;
    bool m_bothDirections = false;

    // The signal at the end of this track element
    Signal *m_signal = NULL;
};

#endif // TRACKELEMENT_H

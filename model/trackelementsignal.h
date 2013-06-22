#ifndef TRACKELEMENTSIGNAL_H
#define TRACKELEMENTSIGNAL_H

#include <QString>

class TrackElementSignal
{
public:
    TrackElementSignal();

    inline QString stationName() const { return m_stationName; }
    inline QString signalName() const { return m_signalName; }

    inline void setStationName(const QString value) { m_stationName = value; }
    inline void setSignalName(const QString value) { m_signalName = value; }

private:
    QString m_stationName;
    QString m_signalName;
};

#endif // TRACKELEMENTSIGNAL_H

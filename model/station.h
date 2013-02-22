#ifndef STATION_H
#define STATION_H

/**
 * Contains information about a station (Haltepunkt/Betriebsstelle)
 */
struct Station {

    /**
     * The position of the station.
     */
    QPointF pos;

    /**
     * The name of the station.
     */
    QString name;
};

#endif // STATION_H

#ifndef TIMETABLEENTRY_H
#define TIMETABLEENTRY_H

/**
 * Represents an entry in a train's timetable
 */
struct TimetableEntry {
    /**
     * The station name.
     */
    QString stationName;

    /**
     * The allowed tracks.
     */
    QList<QString> allowedTracks;

    /**
     * The arrival time for that station, or 0 if no arrival time is set.
     * TODO: any timetables on 1970-01-01?
     */
    time_t arrivalTime;

    /**
     * The departure time for that station, or 0 if no departure time is set.
     */
    time_t departureTime;
};

#endif // TIMETABLEENTRY_H

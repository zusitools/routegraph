#ifndef STATICTRAININFO_H
#define STATICTRAININFO_H

#include <QString>
#include <QQueue>
#include <QPair>
#include <QList>

#include "model/trackelement.h"
#include "model/fahrstrassesegment.h"
#include "model/timetableentry.h"

/**
 * This struct contains information about a train that is valid for all instances throughout the
 * entire capture process.
 */
struct StaticTrainInfo
{
public:
    /**
     * The address of the Train object in Zusi.
     */
    uint32_t address;

    /**
     * The type of train, e.g. "ICE"
     */
    QString trainType;

    /**
     * The number of the train.
     */
    QString trainNumber;

    /**
     * The track elements (physically) occupied by this train.
     * This contains all track elements ever occupied by the train and is shared across
     * all instances of this train through time.
     */
    QQueue<TrackElement*> occupiedTrackElements;

    /**
     * The Fahrstrasse segments occupied by this train's Fahrstrasse.
     * This contains all Fahrstrasse segments ever occupied by the train and is shared across
     * all instances of this train through time.
     */
    QQueue<FahrstrasseSegment*> occupiedFahrstrasseSegments;

    /**
     * The timetable of this train. For each timetable entry, contains the station name as a key and
     * the list of allowed tracks as a value.
     */
    QList<TimetableEntry* > timetable;

    /**
     * Temporary, only for serializing!
     * Stores the numbers of the track elements in occupiedTrackElements.
     */
    QQueue<uint32_t> occupiedTrackElementNumbers;

    /**
     * Temporary, only for serializing!
     * Stores the numbers of the start track elements in occupiedFahrstrasseSegments.
     */
    QQueue<uint32_t> occupiedFahrstrasseSegmentStartNumbers;

    /**
     * Saves information about a StaticTrainInfo to a QDataStream
     */
    friend QDataStream &operator<< (QDataStream& stream, const StaticTrainInfo* staticTrainInfo);

    /**
     * Loads information about a StaticTrainInfo to a QDataStream
     */
    friend QDataStream &operator>> (QDataStream& stream, StaticTrainInfo*& staticTrainInfo);
};

#endif // STATICTRAININFO_H

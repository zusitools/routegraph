#ifndef TRAIN_H
#define TRAIN_H

#include <QString>
#include <QList>
#include <QQueue>
#include <QDataStream>

#include "model/trackelement.h"
#include "shareddeque.h"

struct Train
{
    /**
     * We do need to have a constructor here since VC++ does not allow data member initializers.
     */
    Train();

    /**
     * The copy constructor which does NOT set the usage count. acquire() has to be called manually!
     */
    Train(const Train *other);
    ~Train();

    /**
     * Compares two trains against each other.
     */
    bool operator==(const Train &other) const {
        return this->onTrack == other.onTrack && this->isPassengerTrain == other.isPassengerTrain && this->isPlannedStop == other.isPlannedStop
                && this->startX == other.startX && this->startY == other.startY && this->endX == other.endX && this->endY == other.endY
                && this->speed == other.speed && this->fahrstrasseStartPtr == other.fahrstrasseStartPtr && this->fahrstrasseEndPtr == other.fahrstrasseEndPtr;
    }

    bool operator!=(const Train &other) const { return !(*this == other); }

    /**
     * The simulation time at which this Train instance was captured.
     */
    time_t simTime;

    /**
     * The previously captured instance of the same train.
     */
    Train *prev;

    /**
     * The next captured instance of the same train.
     */
    Train *next;

    /**
     * The coordinates of the train head and tail.
     */
    float startX, startY, endX, endY;

    /**
     * The current speed in m/s
     */
    float speed;

    /**
     * Current train length in m.
     */
    float length;

    bool onTrack;
    bool isPassengerTrain;

    /**
     * Whether this passenger train is currently at a planned stop in a station
     */
    bool isPlannedStop;

    /**
     * The number of the next planned stop in the timetable.
     */
    uint32_t nextPlannedStop;

    /**
     * The number of the timetable entry to which the route is currently set.
     */
    uint32_t currentTimetableEntry;

    /**
     * The track elements occupied by this train.
     */
    SharedDeque<TrackElement*> *occupiedTrackElements;

    /**
     * The pointer to the element at the head of this train.
     */
    uint32_t headElemPtr;

    /**
     * The pointer to the element at the tail of this train.
     */
    uint32_t tailElemPtr;

    /**
     * The number of the element at the head of this train.
     */
    uint32_t headElemNumber;

    /**
     * The number of the element at the tail of this train.
     */
    uint32_t tailElemNumber;

    /**
     * The Fahrstrasse segments occupied by this train's Fahrstrasse.
     */
    SharedDeque<FahrstrasseSegment*> *occupiedFahrstrasseSegments;

    /**
     * The number of the start element of the Fahrstrasse.
     */
    uint32_t fahrstrasseStartElemNumber;

    /**
     * The pointer address of the start element of the Fahrstrasse.
     */
    uint32_t fahrstrasseStartPtr;

    /**
     * The number of the end element of the Fahrstrasse.
     */
    uint32_t fahrstrasseEndElemNumber;

    /**
     * The pointer address of the end element of the Fahrstrasse.
     */
    uint32_t fahrstrasseEndPtr;

    /**
     * ---------1=============2XXXX3>=========4--------
     *
     * 1 = Fahrstrasse end
     * 2 = Train tail
     * 3 = Train head
     * 4 = Fahrstrasse start
     */

    /**
     * Increases this train's usage count.
     */
    void acquire();

    /**
     * Decreases this train's usage count and if the usage count is zero, deletes it.
     */
    void release();

private:
    /**
     * Updates the next and prev pointers of the neighboring elements so that this train is no longer
     * in the implicit linked list of trains.
     */
    void removeFromList();

    /**
     * The usage count of this Train, if it is 0, it may be safely deleted.
     * A train's usage count is incremented e.g. if it is the representative of a given simulation time in the
     * TrainManager's train list or if a TrainItem is displaying it.
     */
    unsigned int m_usage_count;
};

/**
 * Saves information about a train to a QDataStream
 */
QDataStream &operator<< (QDataStream& stream, const Train* train);

/**
 * Loads information about a train from a QDataStream
 */
QDataStream &operator>> (QDataStream& stream, Train*& train);

#endif // TRAIN_H

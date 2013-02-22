#ifndef TRAIN_CPP
#define TRAIN_CPP

#include "model/train.h"

Train::Train() :
    simTime(0),
    prev(NULL),
    next(NULL),
    startX (0),
    startY(0),
    endX(0),
    endY(0),
    speed(0),
    length(0),
    onTrack(false),
    isPassengerTrain(false),
    isPlannedStop(false),
    nextPlannedStop(0),
    currentTimetableEntry(0),
    occupiedTrackElements(NULL),
    headElemPtr(0),
    tailElemPtr(0),
    headElemNumber(0),
    tailElemNumber(0),
    occupiedFahrstrasseSegments(NULL),
    fahrstrasseStartElemNumber(0),
    fahrstrasseStartPtr(0),
    fahrstrasseEndElemNumber(0),
    fahrstrasseEndPtr(0),
    m_usage_count(1)
{
}

Train::~Train() {
    Q_ASSERT(m_usage_count <= 0);
    delete occupiedTrackElements;

    // Test code to spot deleted train info in the debugger
    fahrstrasseStartPtr = 42;
    fahrstrasseEndPtr = 42;
    this->startX = 42.0;
    this->startY = 42.0;
    this->endX = 42.0;
    this->endY = 42.0;
}

void Train::acquire()
{
    this->m_usage_count++;
}

void Train::release()
{
    if (--this->m_usage_count == 0) {
        this->removeFromList();
        delete this;
    }
}

void Train::removeFromList()
{
    if (next) {
        next->prev = this->prev;
    }

    if (prev) {
        prev->next = this->next;
    }
}

QDataStream &operator<< (QDataStream& stream, const Train* train)
{
    stream << (qint64)(train->simTime);
    stream << train->startX;
    stream << train->startY;
    stream << train->endX;
    stream << train->endY;
    stream << train->speed;
    stream << train->length;
    stream << train->onTrack;
    stream << train->isPassengerTrain;
    stream << train->isPlannedStop;
    stream << train->nextPlannedStop;
    stream << train->currentTimetableEntry;

    if (train->onTrack) {
        stream << *train->occupiedTrackElements;
        stream << *train->occupiedFahrstrasseSegments;
    }

    return stream;
}

QDataStream &operator>> (QDataStream& stream, Train*& train)
{
    train = new Train();

    stream >> (qint64&)train->simTime;
    stream >> train->startX;
    stream >> train->startY;
    stream >> train->endX;
    stream >> train->endY;
    stream >> train->speed;
    stream >> train->length;
    stream >> train->onTrack;
    stream >> train->isPassengerTrain;
    stream >> train->isPlannedStop;
    stream >> train->nextPlannedStop;
    stream >> train->currentTimetableEntry;

    if (train->onTrack) {
        train->occupiedTrackElements = new SharedDeque<TrackElement*>();
        stream >> *train->occupiedTrackElements;

        train->occupiedFahrstrasseSegments = new SharedDeque<FahrstrasseSegment*>();
        stream >> *train->occupiedFahrstrasseSegments;
    }

    return stream;
}

#endif // TRAIN_CPP

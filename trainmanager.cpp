#include "trainmanager.h"

#include <QDebug>

#include "model/fahrstrassesegment.h"
#include "shareddeque.h"

#ifdef _WIN32
#include "memread/zusimemoryreaderwindows.h"
#else
#include "memread/zusimemoryreaderlinux.h"
#endif

// #define DEADLOCK_CHECK

TrainManager::TrainManager(Route *route)
    : m_memReader(NULL),
      m_isPaused(false),
      m_trainListAddr(0),
      m_numTrains(0),
      m_minSimTime(0),
      m_maxSimTime(0),
      m_route(route),
      m_isCapturing(false)
{
}

TrainManager::~TrainManager()
{
    m_timer.stop();

    qDeleteAll(m_staticTrainInfo.values());
    m_staticTrainInfo.clear();

    if (m_memReader) {
        delete m_memReader;
    }

    foreach (time_t key, m_trains.keys()) {
        foreach (Train *train, *(m_trains.value(key))) {
            train->release();
        }
    }

    qDeleteAll(m_trains.values());
}

void TrainManager::startCapture()
{
    try {
#ifdef _WIN32
        m_memReader = new ZusiMemoryReaderWindows();
#else
        m_memReader = new ZusiMemoryReaderLinux();
#endif
    } catch (ZusiMemoryReaderException &e) {
        m_memReader = NULL;
        emit trainUpdateException(e);
        return;
    }

    checkZusiRunning();
}

QHash<uint32_t, Train *> TrainManager::trainsAt(time_t simTime)
{
    QHash<uint32_t, Train *> result; // FIXME does this always work?

    foreach (uint32_t trainAddr, m_trains.keys()) {
        if (m_trains[trainAddr]->values().count() == 0) {
            continue;
        } else if (simTime == m_maxSimTime) {
            result.insert(trainAddr, m_trains[trainAddr]->values().last());
        } else if (simTime < m_trains[trainAddr]->keys().first()) {
            result.insert(trainAddr, NULL);
        } else {
            // Find nearest value with the requested time
            QMap<time_t, Train*>::iterator i = m_trains[trainAddr]->lowerBound(simTime);
            if (i == m_trains[trainAddr]->end()) {
                // no such value
                result.insert(trainAddr, m_trains[trainAddr]->values().last()); // TODO
            } else if (i.key() > simTime) {
                // Do not display values for later simulation times, only for earlier ones
                // Because of i.key() > simTime >= keys().first() the expression --i always works.
                result.insert(trainAddr, (--i).value());
            } else {
                result.insert(trainAddr, i.value());
            }
        }
    }

    return result;
}

StaticTrainInfo *TrainManager::staticTrainInfo(uint32_t trainAddr) const
{
    if (m_staticTrainInfo.contains(trainAddr)) {
        return m_staticTrainInfo.value(trainAddr);
    } else {
        return NULL;
    }
}

int TrainManager::indexOfTrain(uint32_t trainAddr)
{
    return m_trainAddrList.indexOf(trainAddr);
}

void TrainManager::writeToFile(QDataStream &stream, time_t minSimTime, time_t maxSimTime)
{
    // Save characteristic numbers/string for route
    stream << m_route->lsFile();
    stream << m_route->trackElements().count();

    // Save static train information
    stream << m_staticTrainInfo;

    // Save dynamic train information for the specified simulation times
    QHash<quint32, QMap<quint64, Train*> > trains;

    foreach (uint32_t key, m_trains.keys()) {
        foreach (time_t key2, m_trains.value(key)->keys()) {
            if (key2 >= minSimTime && key2 <= maxSimTime) {
                trains[(quint32)key].insert((quint64)key2, m_trains.value(key)->value(key2));
            }
        }
    }

    stream << trains;

    // Save min/max sim time
    stream << (qint64)minSimTime;
    stream << (qint64)maxSimTime;
}

void TrainManager::loadFromFile(QDataStream &stream, bool ignoreExceptions)
{
    // Load and compare characteristic numbers/string for route, throw exception
    // if they do not match.
    QString lsFile;
    stream >> lsFile;

    if (lsFile != m_route->lsFile() && !ignoreExceptions) {
        throw QObject::tr(".ls file of capture (%1) does not match .ls file of currently loaded route (%2)").arg(lsFile, m_route->lsFile());
    }

    int trackElementCount;
    stream >> trackElementCount;
    if (trackElementCount != m_route->trackElements().count() && !ignoreExceptions) {
        throw QObject::tr("Track element count of capture (%1) does not match track element count of currently loaded route (%2)").arg(trackElementCount, m_route->trackElements().count());
    }

    // Load static train information
    stream >> m_staticTrainInfo;

    foreach (StaticTrainInfo *sti, m_staticTrainInfo) {
        foreach (uint32_t key, sti->occupiedTrackElementNumbers) {
            sti->occupiedTrackElements.append(m_route->trackElement(key));
        }

        foreach (uint32_t key, sti->occupiedFahrstrasseSegmentStartNumbers) {
            sti->occupiedFahrstrasseSegments.append(m_route->trackElement(key)->fahrstrasseSegment());
        }
    }

    // Load dynamic train information
    QHash<quint32, QMap<quint64, Train*> > trains;
    stream >> trains;

    foreach (quint32 trainAddr, trains.keys()) {
        QMap<time_t, Train* > *trainInfo = new QMap<time_t, Train*>();
        m_trains.insert((uint32_t)trainAddr, trainInfo);

        // Replace track element numbers loaded from the capture file with pointers to the TrackElements / FahrstrasseSegments
        foreach (quint64 simTime, trains.value(trainAddr).keys()) {
            Train *train = trains.value(trainAddr).value(simTime);

            if (train != NULL)
            {
                if (train->occupiedFahrstrasseSegments != NULL) {
                    train->occupiedFahrstrasseSegments->setMasterQueue(&m_staticTrainInfo.value(trainAddr)->occupiedFahrstrasseSegments);
                }

                if (train->occupiedTrackElements != NULL) {
                    train->occupiedTrackElements->setMasterQueue(&m_staticTrainInfo.value(trainAddr)->occupiedTrackElements);
                }
            }

            trainInfo->insert((time_t)simTime, train);
        }
    }

    // Clear track element numbers from static train info, we do not need them any more
    foreach (StaticTrainInfo *sti, m_staticTrainInfo) {
        sti->occupiedTrackElementNumbers.clear();
        sti->occupiedFahrstrasseSegmentStartNumbers.clear();
    }

    // Load min/max sim time
    stream >> (quint64&)m_minSimTime;
    stream >> (quint64&)m_maxSimTime;
}

void TrainManager::timerTick()
{
    try {
        qDebug() << m_memReader->numMemoryReads() << "memory reads for" << m_numTrains << "trains";
        m_memReader->resetNumMemoryReads();

        // Check if the correct route was loaded by comparing the .ls file names (the route name is not stored directly)
        uint32_t routeAddr = m_memReader->routeAddr();
        if (routeAddr == 0) {
            return;
        }

        if (m_memReader->lsFileName(routeAddr) != m_route->lsFile()) {
            if (m_isCapturing) {
                // Stop capture if a different route was loaded
                m_timer.stop();
                m_isCapturing = false;
            }

            // Set trainListAddr to 0 so when the correct route is loaded, we do not stop after the timetable was loaded
            m_trainListAddr = 0;
            return;
        }

        m_isCapturing = true;

        // Check if the current timetable changed. Start the capture if a timetable was loaded,
        // and stop the capture if a different timetable was loaded.
        uint32_t currentTrainListAddr = m_memReader->trainListAddr();

        if (m_trainListAddr == 0) {
            if (currentTrainListAddr != 0) {
                if (!m_memReader->isTrainListAddrValid(currentTrainListAddr)) {
                    throw ZusiMemoryReaderException("Reading train list", QString("Train list not found at specified memory location"));
                }

                m_trainListAddr = currentTrainListAddr;

                // Read track elements with signals for faster lookup of Fahrstrasse end elements
                m_memReader->readTrackElementNumbersBySignalNamePointerAddress(m_route->trackElementsWithSignals());

                // Start capture, initialize min/max sim time
                m_minSimTime = 0;
                m_maxSimTime = 0;
            } else {
                return;
            }
        } else {
            if (currentTrainListAddr != m_trainListAddr) {
                // Stop capture if a different timetable was loaded. The user can manually resume the capture,
                // but we do not want him to lose the current one only because a new timetable was loaded.
                m_timer.stop();
                m_isCapturing = false;
                return;
            }
        }

        // Read simulation time and check if the simulation is paused.
        // If yes, read the information one last time, after that do not read anything until the simulation resumes.
        bool isRunning;
        time_t simTime = m_memReader->simTime(isRunning);

        if (!isRunning) {
            if (m_isPaused) {
                return;
            } else {
                m_isPaused = true;
            }
        } else {
            m_isPaused = false;
        }

        // Update min and max sim time.
        // The simulation time may be wrong when the timetable is loaded, but the simulation
        // has not yet started. Therefore, set minSimTime only if we already captured train information
        // (which implies the simulation is running).
        if (m_minSimTime == 0 || m_trains.count() == 0) {
            m_minSimTime = simTime;
        }

        m_maxSimTime = simTime;

        // Retrieve number of trains and corresponding addresses
        unsigned int numTrains;
        uint32_t trainListHeadAddr;

        m_memReader->beginAtomicRead();
        m_memReader->readTrainListInfo(m_trainListAddr, trainListHeadAddr, numTrains);

        // If number of trains changed, read the new train list.
        if (numTrains != m_numTrains) {
            QList<uint32_t> trainAddrList = m_memReader->trainAddrs(trainListHeadAddr, numTrains);
            QSet<uint32_t> trainAddrs = QSet<uint32_t>::fromList(trainAddrList);

            // Find out which trains have been deleted in the meantime
            QSet<uint32_t> deletedTrains = m_trainAddrs - trainAddrs;
            foreach (uint32_t trainAddr, deletedTrains) {
                // Mark Fahrstrasse segments as not occupied
                StaticTrainInfo *staticTrainInfo = m_staticTrainInfo[trainAddr];
                foreach (FahrstrasseSegment *fs, staticTrainInfo->occupiedFahrstrasseSegments) {
                    fs->setIsOccupied(false);
                }

                // Insert a last train item with onTrack = false.
                Train *train = new Train();
                train->onTrack = false;
                m_trains[trainAddr]->insert(simTime, train);
            }

            // Find out which trains have been inserted in the meantime.
            // (Note trains are not inserted into the list during simulation but rather
            // when the timetable is loaded.)
            QSet<uint32_t> newTrains = trainAddrs - m_trainAddrs;
            foreach (uint32_t trainAddr, newTrains) {
                m_trainAddrs.insert(trainAddr);

                // Read static train information (only once per train)
                StaticTrainInfo *staticTrainInfo = new StaticTrainInfo;
                m_memReader->readStaticTrainInfo(trainAddr, staticTrainInfo);
                m_staticTrainInfo.insert(trainAddr, staticTrainInfo);
            }

            m_numTrains = numTrains;
            m_trainAddrList = trainAddrList;
        }

        m_memReader->endAtomicRead();

        foreach (uint32_t trainAddr, m_trainAddrs) {
            // Do not read info for a train that which is not yet scheduled to be set on track.
            StaticTrainInfo *staticTrainInfo = m_staticTrainInfo.value(trainAddr);

            // Allow for a 1 second error margin.
            if (staticTrainInfo->timetable.count() > 0 && staticTrainInfo->timetable.first()->arrivalTime > simTime + 1) {
                // In debug mode, check that we did not miss a train that was on track.
// #define DEBUG_TRAINS_ON_TRACK
#ifdef DEBUG_TRAINS_ON_TRACK
                Train *train = new Train();
                m_memReader->readTrainInfo(trainAddr, train);
                Q_ASSERT(!train->onTrack);
                train->release();
#endif
                continue;
            }

            // Read dynamic train information
            Train *train = new Train();
            m_memReader->readTrainInfo(trainAddr, train);

            if (!train->onTrack) {
                // Train is not yet on track (if it was previously, it will have been deleted above)
#ifdef DEBUG_TRAINS_ON_TRACK
                qDebug() << staticTrainInfo->trainType << staticTrainInfo->trainNumber << "not yet on track";
#endif
                train->release();
            } else {
                Train *prev = NULL;

                if (!m_trains.contains(trainAddr)) {
                    m_trains.insert(trainAddr, new QMap<time_t, Train*>);
                } else {
                    // Get info from previous capture
                    prev = m_trains[trainAddr]->values().last();
                }

                // Store the train info only if it changed
                if (prev == NULL || *train != *prev) {
                    m_trains[trainAddr]->insert(simTime, train);
                } else {
                    train->release();
                    continue;
                }

                train->prev = prev;
                train->simTime = simTime;

                if (prev) {
                    prev->next = train;
                }

                // Copy track element and Fahrstrasse data from prev and update it if applicable
                train->occupiedFahrstrasseSegments = prev != NULL ?
                            new SharedDeque<FahrstrasseSegment*>(prev->occupiedFahrstrasseSegments) :
                            new SharedDeque<FahrstrasseSegment*>(&staticTrainInfo->occupiedFahrstrasseSegments);

                train->occupiedTrackElements = prev != NULL ?
                            new SharedDeque<TrackElement*>(prev->occupiedTrackElements) :
                            new SharedDeque<TrackElement*>(&staticTrainInfo->occupiedTrackElements);

                // Read numbers of Fahrstrasse start and end elements
                if (prev == NULL || train->fahrstrasseStartPtr != prev->fahrstrasseStartPtr) {
                    if (train->fahrstrasseStartPtr != 0) {
                        train->fahrstrasseStartElemNumber = m_memReader->trackElementNumberBySignalNamePointerAddress(train->fahrstrasseStartPtr);
                    } else {
                        train->fahrstrasseStartElemNumber = 0;
                    }
                } else {
                    train->fahrstrasseStartElemNumber = prev->fahrstrasseStartElemNumber;
                }

                if (prev == NULL || train->fahrstrasseEndPtr != prev->fahrstrasseEndPtr) {
                    train->fahrstrasseEndElemNumber = m_memReader->trackElementNumberByPointerAddress(train->fahrstrasseEndPtr);
                } else {
                    train->fahrstrasseEndElemNumber = prev->fahrstrasseEndElemNumber;
                }

                // Read numbers of head and tail elements
                if (prev == NULL || train->headElemPtr != prev->headElemPtr) {
                    train->headElemNumber = m_memReader->trackElementNumberByPointerAddress(train->headElemPtr);
                } else {
                    train->headElemNumber = prev->headElemNumber;
                }

                if (prev == NULL || train->tailElemPtr != prev->tailElemPtr) {
                    train->tailElemNumber = m_memReader->trackElementNumberByPointerAddress(train->tailElemPtr);
                } else {
                    train->tailElemNumber = prev->tailElemNumber;
                }

                // Update occupied Fahrstrasse segments
                if (train->fahrstrasseEndElemNumber == 0) {
                    throw ZusiMemoryReaderException(QObject::tr("Read Fahrstrasse for train %1 %2").arg(staticTrainInfo->trainType, staticTrainInfo->trainNumber),
                                                    QObject::tr("Fahrstrasse end element number is 0"));
                }

                // The Fahrstrasse segment that belongs to the "end of Fahrstrasse" pointer of the train.
                FahrstrasseSegment *endSegment = m_route->trackElement(train->fahrstrasseEndElemNumber)->fahrstrasseSegment();

                // Release all Fahrstrasse segments until the last occupied one is lastSegment
                while (!train->occupiedFahrstrasseSegments->isEmpty() && train->occupiedFahrstrasseSegments->last() != endSegment) {
                    train->occupiedFahrstrasseSegments->dequeue();
                }

                // An empty queue means that we have to recalculate the Fahrstrasse segment start.
                // (This can happen e.g. after a train turned.)
                bool queueWasEmpty = train->occupiedFahrstrasseSegments->isEmpty();

                // Enqueue at least the segment "firstSegment".
                if (queueWasEmpty) {
                    train->occupiedFahrstrasseSegments->enqueue(endSegment);
                }

                // Enqueue all other segments until the start of the current Fahrstrasse if the fahrstrasseStartPtr has changed or the queue was empty.
                if (train->fahrstrasseStartElemNumber != 0 && (prev == NULL || prev->fahrstrasseStartPtr != train->fahrstrasseStartPtr || queueWasEmpty)) {

                    // When a train has turned, the fahrstrasseEndPtr is not updated and we are searching for the way
                    // in the wrong direction. We have no way of finding out that we are heading in the wrong direction, so
                    // we do not directly allocate the segments but store them in a list.
                    // If the number of signals reached is > the recursion depth of the route we know we went in the wrong direction.

                    FahrstrasseSegment *currentStartSegment = train->occupiedFahrstrasseSegments->first();
                    QQueue<FahrstrasseSegment*> maybeOccupied;
                    int recursionDepth = m_memReader->fahrstrasseRecursionDepth(); // TODO cache this

                    while (recursionDepth >= 0 && currentStartSegment != NULL && currentStartSegment->lastElement()->number() != train->fahrstrasseStartElemNumber) {
                        if (currentStartSegment->lastElement()->directionInfo(true) && currentStartSegment->lastElement()->directionInfo(true)->signal) {
                            recursionDepth--;
                        }

                        currentStartSegment = getNextSegment(currentStartSegment);

                        if (currentStartSegment != NULL) {
                            maybeOccupied.enqueue(currentStartSegment);
                        }
                    }

                    if (currentStartSegment != NULL && currentStartSegment->lastElement()->number() == train->fahrstrasseStartElemNumber) {
                        // we did find a route!
                        foreach (FahrstrasseSegment *segment, maybeOccupied) {
                            train->occupiedFahrstrasseSegments->enqueue(segment);
                        }
                    } else {
                        // Plan B, Fahrstrasse goes until the next signal
                        train->fahrstrasseStartElemNumber = 0;
                    }
                }

                // If no end of the Fahrstrasse is known, the Fahrstrasse goes until the next signal.
                // No "else if" here, we could set fahrstrasseEndElemNumber to zero a few lines above!
                if (train->fahrstrasseStartElemNumber == 0) {
                    FahrstrasseSegment *currentStartSegment = train->occupiedFahrstrasseSegments->first();

                    while (currentStartSegment != NULL && !(currentStartSegment->lastElement()->directionInfo(true) && currentStartSegment->lastElement()->directionInfo(true)->signal)) {
                        currentStartSegment = getNextSegment(currentStartSegment);
                        if (currentStartSegment != NULL) {
                            train->occupiedFahrstrasseSegments->enqueue(currentStartSegment);
                        }
                    }

                    if (currentStartSegment == NULL) {
                        qDebug() << "something went wrong at" << __FILE__ << ":" << __LINE__ << staticTrainInfo->trainType << staticTrainInfo->trainNumber;
                    } else {
                        train->fahrstrasseStartElemNumber = currentStartSegment->lastElement()->number();
                    }
                }

                // Update occupied track elements
                // Dequeue track elements released by the train in the meantime
                while (!train->occupiedTrackElements->isEmpty() && train->occupiedTrackElements->last()->number() != train->tailElemNumber) {
                    train->occupiedTrackElements->dequeue();
                }

                // If the train length decreases, wagons have been removed. Removing is done preserving the train's tail element,
                // so the head index has to be moved back.
                if (prev != NULL && train->length < prev->length) {
                    while (!train->occupiedTrackElements->isEmpty() && train->occupiedTrackElements->first()->number() != train->headElemNumber) {
                        train->occupiedTrackElements->dequeueFromStart();
                    }
                }

                // Enqueue new track elements.

                // Enqueue at least the element at the tail of the train and work from there
                if (train->occupiedTrackElements->isEmpty()) {
                    if (train->occupiedTrackElements->atEnd()) {
                        train->occupiedTrackElements->enqueue(m_route->trackElement(train->tailElemNumber));
                    } else {
                        train->occupiedTrackElements->enqueue();
                    }
                }

                // Follow the occupied Fahrstrasse segments until finding the element at the train's head.
                TrackElement *currentHeadElement = train->occupiedTrackElements->first();

                while (currentHeadElement->number() != train->headElemNumber) {
                    if (!train->occupiedTrackElements->atEnd()) {
                        // Follow the track elements previously occupied before decreasing the train's length.
                        train->occupiedTrackElements->enqueue();
                        currentHeadElement = train->occupiedTrackElements->first();
                        continue;
                    } else if (currentHeadElement->next.size() == 0) {
                        qDebug() << "something went wrong at" << __FILE__ << ":" << __LINE__ << staticTrainInfo->trainType << staticTrainInfo->trainNumber;
                        break;
                    } else if (currentHeadElement->next.size() == 1) {
                        currentHeadElement = currentHeadElement->next.front();
                    } else {
                        // more than one choice, read from Fahrstrasse segments
                        FahrstrasseSegment *segment = currentHeadElement->fahrstrasseSegment();

                        // Use lastIndexOf, else we run into problems when a train uses one segment twice.
                        int idx = staticTrainInfo->occupiedFahrstrasseSegments.lastIndexOf(segment);

                        if (idx == -1 || idx == staticTrainInfo->occupiedFahrstrasseSegments.count() - 1) {
                            qDebug() << "something went wrong at" << __FILE__ << ":" << __LINE__ << idx << staticTrainInfo->trainType << staticTrainInfo->trainNumber;
                            break;
                        } else {
                            currentHeadElement = staticTrainInfo->occupiedFahrstrasseSegments.at(idx + 1)->firstElement();
                        }
                    }

                    train->occupiedTrackElements->enqueue(currentHeadElement);
                }

#ifdef DEADLOCK_CHECK
                // Execute route search for deadlock check if train stopped
                // TODO: what if we catch the time between v=0 and execution of special actions, e.g. turning around?
                if (train->speed == 0.0 && (prev == NULL || prev->speed > 0.0)) {
                    qDebug() << "train" << staticTrainInfo->trainType << staticTrainInfo->trainNumber << "stopped";

                    if (train->isPlannedStop && train->nextPlannedStop == train->currentTimetableEntry) {
                        qDebug() << "stopped at current timetable entry";
                    } else {
                        // TODO check distance to next signal
                    }

                    if (train->fahrstrasseStartElemNumber != 0 && train->currentTimetableEntry < (unsigned int)(staticTrainInfo->timetable.length() - 1)) {
                        TimetableEntry *nextTimetableEntry = staticTrainInfo->timetable.at(train->currentTimetableEntry + 1);
                        qDebug() << "searching route from" << train->fahrstrasseStartElemNumber << "to" << nextTimetableEntry->stationName << nextTimetableEntry->allowedTracks;

                        QList<QList<FahrstrasseSegment*> *> routes = m_route->findRoutesTo(train->fahrstrasseStartElemNumber, nextTimetableEntry, m_memReader->fahrstrasseRecursionDepth());
                        qDebug() << routes;
                    }

                    // TODO emit trainStoppedSignal
                }
#endif

                // Delete "prev" if it has the same simulation time as "train" since "train" will now be the new representative
                // for this simulation time.
                if (prev != NULL && prev->simTime == train->simTime) {
                    prev->release();
                }
            }
        }

        emit trainsUpdated();

    } catch (ZusiMemoryReaderException &e) {
        m_isCapturing = false;
        m_timer.stop();

        // Do not rethrow the exception if Zusi was just closed.
        if (m_memReader->isZusiRunning()) {
            emit trainUpdateException(e);
        }
    }
}

void TrainManager::checkZusiRunning()
{
    try {
        if (m_memReader->isZusiRunning()) {
            m_memReader->startCapture();

            // Read first capture right away
            timerTick();

            connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
            m_timer.start(100);
        } else {
            // If Zusi is not running, check back every second
            m_timer.singleShot(1000, this, SLOT(checkZusiRunning()));
        }
    } catch (ZusiMemoryReaderException &e) {
        m_timer.stop();
        emit trainUpdateException(e);
    }
}

FahrstrasseSegment *TrainManager::getNextSegment(FahrstrasseSegment *currentSegment)
{
    TrackElement *currentLastElem = currentSegment->trackElements()->last();

    if (currentLastElem->next.size() == 0) {
        return NULL;
    } else if (currentLastElem->next.size() == 1) {
        return currentLastElem->next.front()->fahrstrasseSegment();
    } else {
        // multiple successors, read the correct one from memory
        return m_route->trackElement(m_memReader->successorElementNumber(currentLastElem->number()))->fahrstrasseSegment();
    }
}

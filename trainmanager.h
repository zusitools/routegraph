#ifndef TRAINMANAGER_H
#define TRAINMANAGER_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QTimer>

#include "memread/zusimemoryreader.h"
#include "memread/zusimemoryreaderexception.h"
#include "model/train.h"
#include "model/statictraininfo.h"
#include "model/route.h"

class TrainManager : public QObject
{
    Q_OBJECT

public:
    TrainManager(Route *route);
    ~TrainManager();

    /**
     * Starts the capture of the Zusi process (if Zusi is not running, waits for Zusi to appear).
     */
    void startCapture();

    /**
     * Returns the train infos as they were at the given time.
     */
    QHash<uint32_t, Train*> trainsAt(time_t simTime);

    /**
     * Returns the static train info for the train with the given address.
     */
    StaticTrainInfo *staticTrainInfo(uint32_t trainAddr) const;

    /**
     * Returns the minimum recorded simulation time.
     */
    inline time_t minSimTime() { return m_minSimTime; }

    /**
     * Returns the maximum recorded simulation time.
     */
    inline time_t maxSimTime() { return m_maxSimTime; }

    /**
     * Returns the index of the train with the specified address in Zusi's train list.
     */
    int indexOfTrain(uint32_t trainAddr);

    /**
     * Saves information about a capture to a QDataStream
     */
    void writeToFile(QDataStream& stream, time_t minSimTime, time_t maxSimTime);

    /**
     * Loads information about a capture from a QDataStream
     */
    void loadFromFile(QDataStream& stream, bool ignoreExceptions = false);

signals:
    /**
     * This signal is emitted when a new capture frame has been read from Zusi.
     */
    void trainsUpdated();

    /**
     * This signal is emitted when a memory read exception occurred during the update of the train.
     * @param e The exception that occurred.
     */
    void trainUpdateException(ZusiMemoryReaderException &e);

private:
    /**
     * The memory reader responsible for performing the read of the Zusi process.
     */
    ZusiMemoryReader* m_memReader;

    /**
     * The train information. For each train (identified by its memory address), contains a map of the train information
     * at different capture times.
     */
    QHash<uint32_t, QMap<time_t, Train*>*> m_trains;

    /**
     * The addresses of the trains currently in the simulation
     */
    QSet<uint32_t> m_trainAddrs;

    /**
     * The addresses of the trains currently in the simulation in proper order.
     */
    QList<uint32_t> m_trainAddrList;

    /**
     * For each train (identified by its memory address), contains the static train information for that train.
     */
    QHash<uint32_t, StaticTrainInfo*> m_staticTrainInfo;

    /**
     * The timer responsible for periodic reading of the Zusi process.
     */
    QTimer m_timer;

    /**
     * Whether the simulation was paused at the time of the last capture.
     */
    bool m_isPaused;

    /**
     * The address of the train list, used to detect timetable (re)loads (since it changes
     * when a new timetable is loaded).
     */
    uint32_t m_trainListAddr;

    /**
     * The number of trains in the simulation at the time of the last capture.
     */
    unsigned int m_numTrains;

    /**
     * Returns the segment following the given Fahrstrasse segment, taking into account
     * the current position of turnouts.
     */
    FahrstrasseSegment *getNextSegment(FahrstrasseSegment *currentSegment);

    /**
     * The minimum recorded simulation time.
     */
    time_t m_minSimTime;

    /**
     * The maximum recorded simulation time.
     */
    time_t m_maxSimTime;

    /**
     * The route we are working with.
     */
    Route *m_route;

    /**
     * Whether we are currently capturing.
     */
    bool m_isCapturing;

private slots:
    /**
     * Captures a new frame and emits the trainsUpdated() signal.
     */
    void timerTick();

    /**
     * Checks whether Zusi is running and if yes, starts the capture.
     */
    void checkZusiRunning();
};

#endif // TRAINMANAGER_H

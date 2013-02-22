#ifndef ZUSIMEMORYREADER_H
#define ZUSIMEMORYREADER_H

#include <stdint.h>
#include <QHash>
#include <QSet>

#include "model/train.h"
#include "model/statictraininfo.h"

/**
 * DUMMY IMPLEMENTATION -- THIS STUB IMPLEMENTATION IS DESIGNED TO KEEP THE REST
 * OF THE CODE RUNNING ALTHOUGH THE MEMORY READ ROUTINES ARE NOT PUBLISHED.
 */
class ZusiMemoryReader
{
public:
    ZusiMemoryReader();
    virtual ~ZusiMemoryReader();

    /**
     * Returns the current simulation time and whether the simulation is running at the moment.
     */
    time_t simTime(bool &isSimulationRunning);

    /**
     * Returns the memory address of the train list.
     */
    uint32_t trainListAddr();

    /**
     * Returns whether there is a valid train list at the specified address.
     */
    bool isTrainListAddrValid(uint32_t trainListAddr);

    /**
     * Reads the information (list head address, list size) from the train list at the specified address.
     */
    void readTrainListInfo(uint32_t trainListAddr, uint32_t &trainListHeadAddr, unsigned int &numTrains);

    /**
     * Returns the memory address of the object containing the route information
     */
    uint32_t routeAddr();

    /**
     * Returns the path of the .ls file of the currently loaded route as specified in the route's .str file.
     */
    QString lsFileName(uint32_t routeAddr);

    /**
     * Returns a list with the addresses of the trains currently in the simulation.
     * The list will be in the same order as the train list in Zusi.
     *
     * @param trainListHeadAddr The address of the head element of the train list
     * @param numTrains The number of trains
     */
    QList<uint32_t> trainAddrs(uint32_t trainListHeadAddr, uint32_t numTrains);

    /**
     * Reads the dynamic information (speed, position etc.) about the train with the specified address
     * into the provided Train structure.
     */
    void readTrainInfo(uint32_t trainAddr, Train *train);

    /**
     * Reads the static train info for the train with the given address.
     */
    void readStaticTrainInfo(uint32_t trainAddr, StaticTrainInfo *staticTrainInfo);

    /**
     * Returns the number of the current successor element of the track element with the given number.
     */
    uint32_t successorElementNumber(uint32_t elementNumber);

    /**
     * Returns the recursion depth of the route that is applied when looking for a Fahrstrasse.
     */
    uint32_t fahrstrasseRecursionDepth();

    /**
     * Returns the number of a track element by the address of a pointer to it.
     */
    uint32_t trackElementNumberByPointerAddress(uint32_t pointerAddress);

    /**
     * Returns the number of a track element by the address of its signal name pointer.
     * readTrackElementNumbersBySignalNamePointerAddress has to be called beforehand.
     */
    uint32_t trackElementNumberBySignalNamePointerAddress(uint32_t pointerAddress);

    /**
     * Fills the "elements by signal name pointer address" cache with the provided elements.
     */
    void readTrackElementNumbersBySignalNamePointerAddress(QList<TrackElement *> trackElementsWithSignals);

    /**
     * Begins an atomic read operation – the program will be stopped until endAtomicRead().
     */
    virtual void beginAtomicRead() = 0;

    /**
     * Ends an atomic read operation.
     */
    virtual void endAtomicRead() = 0;

    /**
     * Returns the number of memory accesses since the last reset.
     */
    inline unsigned int numMemoryReads() { return m_numMemoryReads; }

    /**
     * Resets the number of memory accesses.
     */
    inline void resetNumMemoryReads() { m_numMemoryReads = 0; }

    /**
     * Returns whether Zusi is running at the moment.
     */
    virtual bool isZusiRunning() = 0;

    /**
     * Sets up everything needed for the capture.
     */
    virtual void startCapture() = 0;

protected:
    /**
     * Specified whether the memory reader is currently attached to the Zusi process.
     */
    bool m_isAttached;

    /**
     * Performs the actual memory reading operation.
     * @param address (Virtual) Address in the Zusi process address space to read from
     * @param len Length of data to read, in bytes.
     * @param buf Pointer to buffer to read data into.
     */
    virtual void doReadMem(int address, int len, void* buf) = 0;

    /**
     * Reads the memory of the Zusi process.
     * @see doReadMem
     */
    void readMem(int address, int len, void* buf);

    /**
     * The number of memory reads since the last reset.
     */
    unsigned int m_numMemoryReads;
};

#endif // ZUSIMEMORYREADER_H

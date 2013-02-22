#include "zusimemoryreader.h"

#include <QDebug>
#include <time.h>
#include "zusimemoryreaderexception.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

ZusiMemoryReader::ZusiMemoryReader() :
    m_isAttached(false),
    m_numMemoryReads(0)
{
}

ZusiMemoryReader::~ZusiMemoryReader()
{
}

time_t ZusiMemoryReader::simTime(bool &isSimulationRunning) {
    return 0;
}

uint32_t ZusiMemoryReader::trainListAddr()
{
    return 0;
}

bool ZusiMemoryReader::isTrainListAddrValid(uint32_t trainListAddr)
{
    return false;
}

void ZusiMemoryReader::readTrainListInfo(uint32_t trainListAddr, uint32_t &trainListHeadAddr, unsigned int &numTrains)
{
}

uint32_t ZusiMemoryReader::routeAddr()
{
    return 0;
}

QString ZusiMemoryReader::lsFileName(uint32_t routeAddr)
{
    return QString();
}

QList<uint32_t> ZusiMemoryReader::trainAddrs(uint32_t trainListHeadAddr, uint32_t numTrains)
{
    return QList<uint32_t>();
}

void ZusiMemoryReader::readTrainInfo(uint32_t trainAddr, Train *train)
{
}

void ZusiMemoryReader::readStaticTrainInfo(uint32_t trainAddr, StaticTrainInfo *staticTrainInfo)
{
}

uint32_t ZusiMemoryReader::successorElementNumber(uint32_t elementNumber) {
    return 0;
}

uint32_t ZusiMemoryReader::fahrstrasseRecursionDepth()
{
    return 0;
}

unsigned int ZusiMemoryReader::trackElementNumberByPointerAddress(uint32_t pointerAddress)
{
    return 0;
}

uint32_t ZusiMemoryReader::trackElementNumberBySignalNamePointerAddress(uint32_t pointerAddress)
{
    return 0;
}

void ZusiMemoryReader::readTrackElementNumbersBySignalNamePointerAddress(QList<TrackElement*> trackElementsWithSignals)
{
}


/* == Protected members == */

void ZusiMemoryReader::readMem(int address, int len, void *buf)
{
}

#pragma GCC diagnostic pop

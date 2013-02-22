#include "zusimemoryreaderlinux.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

ZusiMemoryReaderLinux::ZusiMemoryReaderLinux() : ZusiMemoryReader()
{
}


void ZusiMemoryReaderLinux::startCapture()
{
}

void ZusiMemoryReaderLinux::beginAtomicRead()
{
}

void ZusiMemoryReaderLinux::doReadMem(int address, int len, void *buf)
{
}

void ZusiMemoryReaderLinux::endAtomicRead()
{
}

bool ZusiMemoryReaderLinux::isZusiRunning()
{
    return false;
}

#pragma GCC diagnostic pop

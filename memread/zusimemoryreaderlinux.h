#ifndef ZUSIMEMORYREADERLINUX_H
#define ZUSIMEMORYREADERLINUX_H

#include "memread/zusimemoryreader.h"

/**
 * DUMMY IMPLEMENTATION -- THIS STUB IMPLEMENTATION IS DESIGNED TO KEEP THE REST
 * OF THE CODE RUNNING ALTHOUGH THE MEMORY READ ROUTINES ARE NOT PUBLISHED.
 */
class ZusiMemoryReaderLinux : public ZusiMemoryReader
{
public:
    void startCapture();
    bool isZusiRunning();

    ZusiMemoryReaderLinux();
    void beginAtomicRead();
    void doReadMem(int address, int len, void *buf);
    void endAtomicRead();
};

#endif // ZUSIMEMORYREADERLINUX_H

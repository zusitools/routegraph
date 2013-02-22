#pragma once
#include "memread/zusimemoryreader.h"

#include <Windows.h>

/**
 * DUMMY IMPLEMENTATION -- THIS STUB IMPLEMENTATION IS DESIGNED TO KEEP THE REST
 * OF THE CODE RUNNING ALTHOUGH THE MEMORY READ ROUTINES ARE NOT PUBLISHED.
 */
class ZusiMemoryReaderWindows :
	public ZusiMemoryReader
{
public:
	ZusiMemoryReaderWindows(void);
	~ZusiMemoryReaderWindows(void);

	void beginAtomicRead();
	void doReadMem(int address, int len, void* buf);
	void endAtomicRead();

	void startCapture();
	bool isZusiRunning();
};


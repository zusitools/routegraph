#include "zusimemoryreaderexception.h"

ZusiMemoryReaderException::ZusiMemoryReaderException(QString action, QString message) : exception()
{
    this->m_action = action;
    this->m_message = message;
}

#if __cplusplus > 199711L
ZusiMemoryReaderException::~ZusiMemoryReaderException() noexcept
#else
ZusiMemoryReaderException::~ZusiMemoryReaderException() throw()
#endif
{
}

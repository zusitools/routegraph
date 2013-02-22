#ifndef ZUSIMEMORYREADEREXCEPTION_H
#define ZUSIMEMORYREADEREXCEPTION_H

#include <exception>
#include <QString>

class ZusiMemoryReaderException : public std::exception
{
public:
    ZusiMemoryReaderException(QString action, QString message);
#if __cplusplus > 199711L
    ~ZusiMemoryReaderException() noexcept;
#else
    ~ZusiMemoryReaderException();
#endif

    inline QString action() { return m_action; }
    inline QString message() { return m_message; }

private:
    QString m_action;
    QString m_message;
};

#endif // ZUSIMEMORYREADEREXCEPTION_H

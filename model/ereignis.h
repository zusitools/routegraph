#ifndef EREIGNIS_H
#define EREIGNIS_H

#include <stdint.h>
#include <QString>

class Ereignis
{
public:
    Ereignis();

    inline uint32_t type() const { return m_type; }
    inline uint32_t value() const { return m_value; }
    inline QString text() const { return m_text; }

    void setType(uint32_t value) { m_type = value; }
    void setValue(uint32_t value) { m_value = value; }
    void setText(QString value) { m_text = value; }

private:
    uint32_t m_type;
    uint32_t m_value;
    QString m_text;
};

#endif // EREIGNIS_H

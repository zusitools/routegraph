#include "register.h"

Register::Register(int regNumber, QObject *parent) :
    QObject(parent),
    m_regNumber(regNumber),
    m_occupationState(Free)
{
}

void Register::setOccupationState(OccupationState value)
{
    if (value == m_occupationState)
    {
        return;
    }

    m_occupationState = value;
    emit occupationChanged(value);
}

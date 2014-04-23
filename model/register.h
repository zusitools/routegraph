#ifndef REGISTER_H
#define REGISTER_H

#include <QObject>

class Register : public QObject
{
    Q_OBJECT
public:
    explicit Register(int regNumber, QObject *parent = 0);

    enum OccupationState
    {
        Occupied,
        Free
    };

    void setOccupationState(OccupationState value);
    inline OccupationState getOccupationState() { return m_occupationState; }

    inline int getRegNumber() { return m_regNumber; }

signals:
    void occupationChanged(OccupationState occupationState);

public slots:

private:
    int m_regNumber;
    OccupationState m_occupationState;
};

#endif // REGISTER_H

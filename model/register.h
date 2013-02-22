#ifndef REGISTER_H
#define REGISTER_H

#include <QObject>
#include "model/train.h"

class Register : public QObject
{
    Q_OBJECT
public:
    explicit Register(QObject *parent = 0);

    inline void setOccupiedBy(Train *occupiedBy) { m_occupiedBy = occupiedBy; }
    inline Train *occupiedBy() { return m_occupiedBy; }

signals:

public slots:

private:
    Train *m_occupiedBy;


};

#endif // REGISTER_H

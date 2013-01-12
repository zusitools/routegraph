#ifndef TRACKSEGMENT_H
#define TRACKSEGMENT_H

#include <QGraphicsPathItem>

class TrackSegment : public QGraphicsPathItem
{
public:
    TrackSegment(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // TRACKSEGMENT_H

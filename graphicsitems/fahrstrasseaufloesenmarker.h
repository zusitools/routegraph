#ifndef FAHRSTRASSEAUFLOESENMARKER_H
#define FAHRSTRASSEAUFLOESENMARKER_H

#include <QGraphicsItem>

class FahrstrasseAufloesenMarker : public QGraphicsItem
{
public:
    FahrstrasseAufloesenMarker(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // FAHRSTRASSEAUFLOESENMARKER_H

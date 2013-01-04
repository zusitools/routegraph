#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include <QGraphicsItem>
#include <QString>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>

#define VIEWPOINTSIZE 50

class ViewPoint : public QGraphicsItem
{
public:
    ViewPoint(const QString name, const float phiZ, QGraphicsItem *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

private:
    float phiZ;
};

#endif // VIEWPOINT_H

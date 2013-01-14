#ifndef WENDEPUNKT_H
#define WENDEPUNKT_H

#include <QGraphicsItem>

class Wendepunkt : public QGraphicsItem
{
public:
    Wendepunkt(QGraphicsItem *parent = 0, qreal phi = 0);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    qreal m_angle;
};

#endif // WENDEPUNKT_H

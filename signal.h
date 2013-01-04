#ifndef SIGNAL_H
#define SIGNAL_H

#include <QGraphicsItem>

class Signal : public QGraphicsItem
{
public:
    Signal(QGraphicsItem *parent, QPointF pos, qreal phi, const QString text);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    float phi;
    QString text;
};

#endif // SIGNAL_H

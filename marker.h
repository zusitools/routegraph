#ifndef MARKER_H
#define MARKER_H

#include <QGraphicsItem>
#include <QColor>

class Marker: public QGraphicsItem
{
public:
    Marker(QGraphicsItem *parent, QPointF pos, qreal phi, const QString text, const QColor color);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    double angle;
    QString text;
    QColor m_color;
};

#endif // MARKER_H

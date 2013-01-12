#ifndef MARKER_H
#define MARKER_H

#include <QGraphicsItem>
#include <QColor>

class Label;

/**
 * Represents a labeled marker shaped as an equilateral triangle.
 * The center of one of the sides is positioned at (0, 0) and the triangle points in the
 * direction specified by the angle.
 */
class Marker: public QGraphicsItem
{
public:
    Marker(QGraphicsItem *parent, QPointF pos, qreal phi, const QString text, const QColor color);
    ~Marker();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Label *label() { return m_label; }
private:
    double angle;
    QString text;
    QColor m_color;

    Label *m_label;
};

#endif // MARKER_H

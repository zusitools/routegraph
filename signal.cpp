#include "signal.h"

#include <QPainter>
#include <QDebug>
#include <limits>

#include "label.h"

#define ARROWSIZE 8
#define ARROWANGLE M_PI / 3

Signal::Signal(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text)
    : QGraphicsItem(parent)
{
    this->setPos(pos);
    this->setZValue(std::numeric_limits<double>::infinity());
    this->phi = phi;
    this->text = text;

    Label *label = new Label(text, this);
    label->setBrush(QBrush(Qt::red));
}

QRectF Signal::boundingRect() const
{
    return QRectF(-ARROWSIZE, -ARROWSIZE, 2 * ARROWSIZE, 2 * ARROWSIZE);
}

void Signal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    float angle = M_PI + phi / 180 * M_PI;

    QPointF origin = QPointF(0, 0);
    const QPointF points[3] = {
        origin,
        origin + QPointF(sin(angle + ARROWANGLE) * ARROWSIZE, cos(angle + ARROWANGLE) * ARROWSIZE),
        origin + QPointF(sin(angle + M_PI - ARROWANGLE) * ARROWSIZE, cos(angle + M_PI - ARROWANGLE) * ARROWSIZE)
    };

    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);

    painter->drawPolygon(points, 3);
}

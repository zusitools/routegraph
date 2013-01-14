#include "marker.h"

#include <QPainter>
#include <QDebug>
#include <limits>
#include <cmath>

#include "label.h"
#include "zvalues.h"

#define ARROWSIZE 8
#define ARROWANGLE (M_PI / 3)

// Height of the equilateral triangle that makes up the marker
#define HEIGHT (ARROWSIZE * sqrt(3.0) / 2)

Marker::Marker(QGraphicsItem *parent, const QPointF pos, qreal phi, const QString text, const QColor color)
    : QGraphicsItem(parent)
{
    this->setPos(pos);
    this->setZValue(ZVALUE_MARKERS);
    this->angle = M_PI / 2 + phi / 180 * M_PI;
    this->text = text;
    this->m_color = color;

    this->m_label = new Label(text, this);
    m_label->setPen(QPen(color));

    // TODO what if phi >= 360?
    if (phi <= 90 - 45 || phi >= 360 - 45) {
        m_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    } else if (phi <= 180 - 45) {
        m_label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    } else if (phi <= 270 - 45) {
        m_label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    } else {
        m_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    }

    QPointF tip = QPointF(HEIGHT * sin(angle), HEIGHT * cos(angle));
    m_label->setPos(tip);
}

Marker::~Marker()
{
    delete m_label;
}

QRectF Marker::boundingRect() const
{
    return QRectF(-ARROWSIZE, -ARROWSIZE, 2 * ARROWSIZE, 2 * ARROWSIZE);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const QPointF points[3] = {
        QPointF(HEIGHT * sin(angle), HEIGHT * cos(angle)),
        QPointF(ARROWSIZE/2 * sin(angle + M_PI / 2), ARROWSIZE/2 * cos(angle + M_PI / 2)),
        QPointF(ARROWSIZE/2 * sin(angle - M_PI / 2), ARROWSIZE/2 * cos(angle - M_PI / 2)),
    };

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);

    painter->drawPolygon(points, 3);
}

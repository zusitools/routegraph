#include "viewpoint.h"

#include <cmath>

#include "label.h"

#define VIEWPOINTSIZE 50

ViewPoint::ViewPoint(QGraphicsItem *parent, const qreal phi, const QString name)
    : QGraphicsItem(parent)
{
    // Add M_PI to the angle to convert to Qt’s coordinate system
    // Angle 0 is pointing downward
    this->m_angle = fmod(phi + M_PI, 2 * M_PI);

    this->m_label = new Label(name, this);
    m_label->setPen(QPen(Qt::blue));

    if (m_angle - M_PI < 0.0001 || m_angle <= M_PI / 2) {
        m_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    } else if (m_angle <= M_PI) {
        m_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    } else if (m_angle <= 3 * M_PI / 2) {
        m_label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    } else {
        m_label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    }
}

ViewPoint::~ViewPoint()
{
    delete m_label;
}

void ViewPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::blue);
    painter->drawLine(0, 0, sin(m_angle + 0.5) * VIEWPOINTSIZE, cos(m_angle + 0.5) * VIEWPOINTSIZE);
    painter->drawLine(0, 0, sin(m_angle - 0.5) * VIEWPOINTSIZE, cos(m_angle - 0.5) * VIEWPOINTSIZE);
}

QRectF ViewPoint::boundingRect() const
{
    return QRectF(-VIEWPOINTSIZE, -VIEWPOINTSIZE, 2 * VIEWPOINTSIZE, 2 * VIEWPOINTSIZE);
}

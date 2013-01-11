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

    Label *label = new Label(name, this);
    label->setPen(QPen(Qt::blue));

    if (m_angle - M_PI < 0.0001 || m_angle <= M_PI / 2) {
        label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    } else if (m_angle <= M_PI) {
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    } else if (m_angle <= 3 * M_PI / 2) {
        label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    } else {
        label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    }
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

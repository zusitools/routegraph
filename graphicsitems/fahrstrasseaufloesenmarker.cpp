#include "fahrstrasseaufloesenmarker.h"

#include <QPainter>
#include "zvalues.h"

#define WIDTH 6.0

FahrstrasseAufloesenMarker::FahrstrasseAufloesenMarker(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent, scene)
{
    this->setZValue(ZVALUE_FAHRSTRASSE_AUFLOESEN_MARKERS);
}

QRectF FahrstrasseAufloesenMarker::boundingRect() const
{
    return QRectF(-WIDTH / 2, 0, WIDTH / 2, WIDTH / 2);
}

static const QPointF points[3] = {
     QPointF(-WIDTH / 2, 0),
     QPointF(0, WIDTH / 2),
     QPointF(WIDTH / 2, 0),
 };

void FahrstrasseAufloesenMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(QBrush(QColor(153, 153, 255)), 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter->drawPolyline(points, 3);
}

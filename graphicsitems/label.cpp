#include "graphicsitems/label.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <limits>

Label::Label(const QString text, QGraphicsItem *parent) : QGraphicsSimpleTextItem(text, parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setZValue(std::numeric_limits<double>::infinity());
}

QRectF Label::boundingRect() const {
    QRectF result = QGraphicsSimpleTextItem::boundingRect();

    if (m_alignment & Qt::AlignRight) {
        result.translate(-result.width(), 0);
    }

    if (m_alignment & Qt::AlignBottom) {
        result.translate(0, -result.height());
    }

    return result;
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->drawText(this->boundingRect(), this->text());
}

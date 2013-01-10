#include "label.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

Label::Label(const QString text, QGraphicsItem *parent) : QGraphicsSimpleTextItem(text, parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
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

QRectF Label::totalBoundingRect()
{
    QRectF boundingRect = this->boundingRect();
    return QRectF(-boundingRect.width(), -boundingRect.height(), 2 * boundingRect.width(), 2 * boundingRect.height());
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->drawText(this->boundingRect(), this->text());

    /* painter->setBrush(Qt::NoBrush);
    painter->drawRect(this->boundingRect());
    painter->drawRect(QRectF(-this->boundingRect().width(), -this->boundingRect().height(),
                             2 * this->boundingRect().width(), 2 * this->boundingRect().height())); */
}

#include "label.h"

#include <QPainter>

Label::Label(const QString text, QGraphicsItem *parent) : QGraphicsSimpleTextItem(text, parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

QRectF Label::totalBoundingRect()
{
    QRectF boundingRect = this->boundingRect();

    return QRectF(-boundingRect.width(), -boundingRect.height(), 2 * boundingRect.width(), 2 * boundingRect.height());
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSimpleTextItem::paint(painter, option, widget);

    painter->setBrush(Qt::NoBrush);
    /* painter->drawRect(QRectF(-this->boundingRect().width(), -this->boundingRect().height(),
                             2 * this->boundingRect().width(), 2 * this->boundingRect().height())); */
}

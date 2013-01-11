#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include <QGraphicsItem>
#include <QString>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>

class ViewPoint : public QGraphicsItem
{
public:
    ViewPoint(QGraphicsItem *parent = 0, const qreal phi = 0, const QString name = QString());

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;

protected:
    qreal m_angle;
};

#endif // VIEWPOINT_H

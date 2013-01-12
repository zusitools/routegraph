#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include <QGraphicsItem>
#include <QString>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>

class Label;

class ViewPoint : public QGraphicsItem
{
public:
    ViewPoint(QGraphicsItem *parent = 0, const qreal phi = 0, const QString name = QString());
    ~ViewPoint();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;

    Label *label() { return m_label; }

protected:
    qreal m_angle;
    Label *m_label;
};

#endif // VIEWPOINT_H

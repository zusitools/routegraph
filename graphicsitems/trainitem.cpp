#include "graphicsitems/trainitem.h"

#include <QPainter>
#include <QPolygonF>
#include <QPen>
#include <QDebug>
#include <cmath>

#include "zvalues.h"
#include "model/trackelementsignal.h"

// Width of the train item (standard scale) in pixels
#define WIDTH 2.5

TrainItem::TrainItem(StaticTrainInfo *staticTrainInfo, bool showTrainName, bool showCurrentSpeed, bool showTargetSignal) : QGraphicsPolygonItem(),
    m_train(NULL),
    m_staticTrainInfo(staticTrainInfo),
    m_label(new Label("", this)),
    m_showTrainName(showTrainName),
    m_showCurrentSpeed(showCurrentSpeed),
    m_showTargetSignal(showTargetSignal),
    m_lastScaleFactor(0)
{
    this->setPen(QPen(Qt::black, 0.2));
    this->setZValue(ZVALUE_TRAINS);

    this->m_label->setPen(QPen(Qt::black));

    // Train item is invisible until first train info is set
    this->setVisible(false);
}

TrainItem::~TrainItem()
{
    delete this->m_label;

    if (this->m_train) {
        this->m_train->release();
    }
}

void TrainItem::setTrain(Train *train)
{
    // Update train usage count
    if (this->m_train) {
        this->m_train->release();
    }

    if (train) {
        train->acquire();
    }

    this->m_train = train;

    if (train == NULL || !train->onTrack) {
        this->setVisible(false);
        return;
    }

    // Set train color
    if (train->isPassengerTrain) {
        if (train->isPlannedStop) {
            this->setBrush(QBrush(QColor(54, 187, 206)));
        } else {
            this->setBrush(QBrush(QColor(3,137,156)));
        }
    } else {
        this->setBrush(QBrush(QColor(166, 80, 0))); // brown
    }

    // Initialize scale factor if necessary
    if (this->m_lastScaleFactor == 0) {
        this->m_lastScaleFactor = QStyleOptionGraphicsItem::levelOfDetailFromTransform(this->sceneTransform());
    }

    this->updateLabel();
    this->updatePolygon();
    this->setVisible(true);
}

void TrainItem::updatePolygon() {
    // The display width of the train item in pixels
    qreal width = WIDTH;

    // Always make the train at least 1 physical pixel thick.
    if (m_lastScaleFactor < 1 / WIDTH) {
        width = (1 / m_lastScaleFactor);
    }

    // TODO check for m_train->onTrack should not be necessary because of this->setVisible(false) in setTrain
    if (m_train == NULL || !m_train->onTrack) {
        return;
    }

    int numPoints = m_train->occupiedTrackElements->count() * 2 + 3;
    QVector<QPointF> points(numPoints);

    // Paint the tail of the train
    TrackElement *te = m_train->occupiedTrackElements->last();
    qreal angle = M_PI / 2 - te->line().angle() / 180 * M_PI;

    points[0] = QPointF(m_train->endX + width * cos(angle), m_train->endY + width * sin(angle));
    points[numPoints - 1] = QPointF(m_train->endX - width * cos(angle), m_train->endY - width * sin(angle));

    // Paint the points at each element
    for (unsigned int i = 1; i < m_train->occupiedTrackElements->count(); i++) {
        te = m_train->occupiedTrackElements->at(i - 1);

        angle = M_PI / 2 - te->line().angle() / 180 * M_PI;

        points[i] = QPointF(te->line().p2().x() + width * cos(angle), te->line().p2().y() + width * sin(angle));
        points[numPoints - 1 - i] = QPointF(te->line().p2().x() - width * cos(angle), te->line().p2().y() - width * sin(angle));
    }

    // Paint the triangle at the head of the train.
    // The angle is taken from the first track element
    QPointF tip = QPointF(m_train->startX + 2 * width * cos(angle - M_PI / 2), m_train->startY + 2 * width * sin(angle - M_PI / 2));

    points[m_train->occupiedTrackElements->count()] = QPointF(m_train->startX + width * cos(angle), m_train->startY + width * sin(angle));
    points[m_train->occupiedTrackElements->count() + 1] = tip;
    points[m_train->occupiedTrackElements->count() + 2] = QPointF(m_train->startX - width * cos(angle), m_train->startY - width * sin(angle));

    // Set label position and alignment
    this->m_label->setPos(QPointF(tip.x() + width, tip.y()));

    this->setPolygon(QPolygonF(points));
}

void TrainItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal scaleFactor = option->levelOfDetailFromTransform(painter->worldTransform());

    if (scaleFactor != m_lastScaleFactor) {
        m_lastScaleFactor = scaleFactor;
        this->updatePolygon();
    }

    QGraphicsPolygonItem::paint(painter, option, widget);
}

void TrainItem::updateLabel()
{
    if (this->m_train == NULL || this->m_staticTrainInfo == NULL || !this->m_train->onTrack /* TODO should not be necessary */) {
        this->m_label->setText("");
        return;
    }

    QString labelText;
    if (this->m_showTrainName) {
        labelText += m_staticTrainInfo->trainType + " " + m_staticTrainInfo->trainNumber;
    }

    if (this->m_showCurrentSpeed) {
        if (!labelText.isEmpty()) {
            labelText += ", ";
        }

        labelText += QString(QObject::tr("v = %1 kph")).arg(QString::number(qRound(m_train->speed * 3.6)));
    }

    if (this->m_showTargetSignal) {
        if (!labelText.isEmpty()) {
            labelText += ", ";
        }

        QString stationRouteFree = m_train->occupiedFahrstrasseSegments->first()->lastElement()->directionInfo(true)->signal->stationName();
        QString trackRouteFree = m_train->occupiedFahrstrasseSegments->first()->lastElement()->directionInfo(true)->signal->signalName();

        labelText += QString(QObject::tr("-> %1, track %2")).arg(stationRouteFree, trackRouteFree);
    }

    this->m_label->setText(labelText);
}

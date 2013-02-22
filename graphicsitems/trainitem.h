#ifndef TRAINITEM_H
#define TRAINITEM_H

#include <QGraphicsPolygonItem>

#include "graphicsitems/label.h"
#include "model/route.h"
#include "model/train.h"
#include "model/statictraininfo.h"

class TrainItem : public QGraphicsPolygonItem
{
public:
    TrainItem(StaticTrainInfo *staticTrainInfo, bool showTrainName = true, bool showCurrentSpeed = true, bool showTargetSignal = true);
    ~TrainItem();

    /**
     * Sets the train information to display.
     */
    void setTrain(Train *train);

    /**
     * Paints this item.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * The currently displayed dynamic train information.
     */
    inline Train *train() { return m_train; }

    /**
     * The static information about the train this train item displays.
     */
    inline StaticTrainInfo *staticTrainInfo() const { return m_staticTrainInfo; }

    /**
     * Returns the label of this train item.
     */
    inline Label *label() const { return m_label; }

    inline void setShowTrainName(bool showTrainName) { m_showTrainName = showTrainName; updateLabel(); }
    inline void setShowCurrentSpeed(bool showCurrentSpeed) { m_showCurrentSpeed= showCurrentSpeed; updateLabel(); }
    inline void setShowTargetSignal(bool showTargetSignal) { m_showTargetSignal = showTargetSignal; updateLabel(); }
private:
    Train *m_train;
    StaticTrainInfo *m_staticTrainInfo;
    Label *m_label;

    bool m_showTrainName;
    bool m_showCurrentSpeed;
    bool m_showTargetSignal;

    /**
     * The level of detail (scale factor) the last time this item was painted.
     */
    qreal m_lastScaleFactor;

    /**
     * Updates the text of the train label
     */
    void updateLabel();

    /**
     * Updates the polygon that makes up the train and the label position
     */
    void updatePolygon();
};

#endif // TRAINITEM_H

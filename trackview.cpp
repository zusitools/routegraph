#include "trackview.h"
#include <cmath>

TrackView::TrackView(QObject *parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void TrackView::scaleBy(double factor)
{
    scale(factor, factor);
}

void TrackView::wheelEvent(QWheelEvent *event)
{
   if (event->modifiers() == Qt::ControlModifier)
    {
        scaleBy(std::pow(4.0 / 3.0, (event->delta() / 240.0)));
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void TrackView::zoomIn()
{
    scaleBy(1.1);
}

void TrackView::zoomOut()
{
    scaleBy(1.0 / 1.1);
}

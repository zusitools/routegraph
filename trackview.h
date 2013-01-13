#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class TrackView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TrackView(QObject *parent = 0);

    void scaleBy(double factor);
    void wheelEvent(QWheelEvent *event);
    void zoomIn();
    void zoomOut();

signals:

public slots:

};

#endif // TRACKVIEW_H

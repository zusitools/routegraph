#include "route.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QPolygonF>
#include <QGraphicsPolygonItem>

#include <cmath>
#include <limits>

#include "signal.h"

void skipLine(QTextStream &ts, int count = 1) {
    for (int i = 0; i < count; i++) {
        ts.readLine();
    }
}

void skipUntil(QTextStream &ts, QString terminator = "#", int count = 1) {
    while (!ts.atEnd() && count > 0) {
        QString line = ts.readLine();

        if (line == terminator) {
            count--;
        }
    }
}

TrackElement* Route::getTrackElement(int number) {
    TrackElement* result = trackElements[number];

    if (result == NULL) {
        result = new TrackElement(number);
        trackElements.insert(number, result);
    }

    return result;
}

Route::Route(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    in.setCodec("ISO 8859-1");

    QHash<int, QList<TrackElement*>*> trackElementsByStartX;
    QList<TrackElement*> trackElementsPointingLeft;
    QHash<TrackElement*, QString> startingPoints;

    // Skip header of file
    skipUntil(in, "#", 2);
    skipLine(in, 2);

    // Read starting points
    while (in.readLine() != "#") {
        int number = in.readLine().toInt();
        startingPoints.insert(getTrackElement(number), in.readLine());
    }

    // Read trackside viewpoints
    QString line;
    while ((line = in.readLine()) != "#") {
        float y = -line.replace(',', '.').toFloat();
        float x = -in.readLine().replace(',', '.').toFloat();
        skipLine(in, 3);

        float phiZ = in.readLine().replace(',', '.').toFloat() + M_PI;
        QString name = in.readLine();

        ViewPoint *vp = new ViewPoint(name, phiZ);
        vp->setPos(x, y);
        m_viewPoints.append(vp);
    }

    double minX = std::numeric_limits<double>::infinity();
    double minY = std::numeric_limits<double>::infinity();
    double maxX = -std::numeric_limits<double>::infinity();
    double maxY = -std::numeric_limits<double>::infinity();

    // Read track element
    while (!in.atEnd()) {
        int elementNumber = in.readLine().toInt();
        TrackElement *te = getTrackElement(elementNumber);
        skipLine(in, 4);

        double startY = -in.readLine().replace(',', '.').toDouble();
        double startX = -in.readLine().replace(',', '.').toDouble();
        double startZ = in.readLine().replace(',', '.').toDouble();

        if (startX < minX) { minX = startX; }
        if (startY < minY) { minY = startY; }
        if (startX > maxX) { maxX = startX; }
        if (startY > maxY) { maxY = startY; }

        double endY = -in.readLine().replace(',', '.').toDouble();
        double endX = -in.readLine().replace(',', '.').toDouble();
        double endZ = in.readLine().replace(',', '.').toDouble();

        // te->setZValue((startZ + endZ) / 2);

        skipLine(in);

        if (endX < minX) { minX = endX; }
        if (endY < minY) { minY = endY; }
        if (endX > maxX) { maxX = endX; }
        if (endY > maxY) { maxY = endY; }

        te->setLine(QLineF(startX, startY, endX, endY));

        int intStartX = (int)startX;
        if (!trackElementsByStartX.contains(intStartX)) {
            QList<TrackElement*> *newList = new QList<TrackElement*>();
            newList->append(te);
            trackElementsByStartX.insert(intStartX, newList);
        } else {
            trackElementsByStartX[intStartX]->append(te);
        }

        //if (startX > endX || ((startX - endX < 0.00001) && startY < endY)) {
        if (te->line().angle() > 90.0 && te->line().angle() <= 270.0) {
            trackElementsPointingLeft.append(te);
        }

        // Read successor elements
        int nextNumber;
        for (int i = 0; i < 3; i++) {
            if ((nextNumber = in.readLine().toInt()) > 0) {
                TrackElement *next = getTrackElement(nextNumber);
                te->next.push_back(next);
                next->prev.push_back(te);
            }
        }

        skipLine(in, 3);

        te->setTunnel(in.readLine().contains("T"));
        te->setElectrified(in.readLine().toInt() > 0);

        // Signal
        if (in.readLine() != "#") {
            skipUntil(in, "#");
            skipLine(in, 3);
        }

        // Kombinationssignal
        if (in.readLine() != "#") {
            skipUntil(in, "#");

            QString station = in.readLine();
            QString signal = in.readLine();

            if (!station.isEmpty() && !signal.isEmpty()) {
                Signal *sig = new Signal(NULL, te->line().p2(), te->line().angle(), station + ", " + signal + " (" + QString::number(te->number()) + ")");
                sig->setPos(te->line().p2());
                m_signals.append(sig);
                te->setHasSignal(true);
            }

            int numRows = in.readLine().toInt() + 1;
            int numCols = in.readLine().toInt() + 1;

            skipLine(in, 5 * numRows + numCols + 1 + 6 * numRows * numCols + 6 + 1);
            skipUntil(in, "#");
            skipLine(in);
        }

        skipLine(in);

        //qDebug() << elemNumber << startX <<startY << endX << endY << next[0] << next[1] << next[2];
        trackElements.insert(te->number(), te);
    }

    qDebug() << minX << minY << maxX << maxY;

    file.close();

    int originalCount = trackElements.count();

    // Remove opposite direction elements
   /* foreach (TrackElement *te, trackElementsPointingLeft) {
        int oppositeStartX = (int)te->line().p2().x();
        if (trackElementsByStartX.contains(oppositeStartX)) {
            foreach (TrackElement *possibleOpposite, *trackElementsByStartX[oppositeStartX]) {
                if (te->isOppositeOf(possibleOpposite)) {
                    trackElements.remove(possibleOpposite->number);
                    te->bothDirections = true;
                    // TODO: Add signal again

                    break;
                }
            }
        }
    }*/

    qDebug() << "Removed" << (originalCount - trackElements.count()) << "elements, now" << trackElements.count();

    foreach (TrackElement *te, trackElements) {
        if (te == NULL) { qDebug() << "NULL"; continue; }
        if (!te->isStartingPointOfSegment()) { continue; }

        QGraphicsPathItem *pathItem = new QGraphicsPathItem();

        QColor penColor = te->electrified() ? Qt::black : Qt::darkGray;
        // penColor = QColor(qrand() % 256, qrand() % 256, qrand() % 256);

        pathItem->setPen(QPen(
                             QBrush(penColor),
                             te->bothDirections() ? 2 : 1,
                             te->tunnel() ? Qt::DotLine : Qt::SolidLine,
                             Qt::FlatCap));

        QVector<QPointF> points;
        points.append(te->line().p1());
        points.append(te->line().p2());

        while (te->next.size() > 0 && te->next.front()->prev.front() == te && !te->next.front()->isStartingPointOfSegment()) {
            te = te->next.front();
            points.append(te->line().p2());

        }

        QPainterPath path;
        path.addPolygon(QPolygonF(points));
        // path.addRect(points.last().x(), points.last().y(), 1, 1);
        pathItem->setPath(path);
        m_trackSegments.append(pathItem);
    }

    qDebug() << m_trackSegments.count() << "track segments";
}

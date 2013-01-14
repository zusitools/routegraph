#include "route.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QHash>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <QGraphicsPolygonItem>

#include <cmath>
#include <limits>

#include "signal.h"
#include "zvalues.h"

// Uncomment this to remove opposite direction elements
// #define REMOVE_OPPOSITES

// Uncomment this to give each segment a random color
// #define RANDOM_COLORS

#define ARROWEVERY 100
#define ARROWLENGTH 10.0
#define ARROWANGLE asin(0.95 / ARROWLENGTH)

void skipLine(QTextStream &ts, int count = 1) {
    for (int i = 0; i < count && !ts.atEnd(); i++) {
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

    #ifdef REMOVE_OPPOSITES
    QHash<int, QList<TrackElement*>*> trackElementsByStartX;
    #endif
    QList<TrackElement*> trackElementsPointingLeft;
    QHash<TrackElement*, QString> startingPoints;

    QString version = in.readLine();
    if (version != "2.2" && version != "2.3" && version != "2.4") {
        throw QString(QObject::tr("Unsupported file format version: %1").arg(version));
    }

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

        float phiZ = in.readLine().replace(',', '.').toFloat();
        QString name = in.readLine();

        ViewPoint *vp = new ViewPoint(NULL, phiZ, name);
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
        skipLine(in, 3);

        int ereignis = in.readLine().toInt();

        double startY = -in.readLine().replace(',', '.').toDouble();
        double startX = -in.readLine().replace(',', '.').toDouble();
        skipLine(in);

        if (startX < minX) { minX = startX; }
        if (startY < minY) { minY = startY; }
        if (startX > maxX) { maxX = startX; }
        if (startY > maxY) { maxY = startY; }

        double endY = -in.readLine().replace(',', '.').toDouble();
        double endX = -in.readLine().replace(',', '.').toDouble();
        skipLine(in);

        skipLine(in);

        if (endX < minX) { minX = endX; }
        if (endY < minY) { minY = endY; }
        if (endX > maxX) { maxX = endX; }
        if (endY > maxY) { maxY = endY; }

        te->setLine(QLineF(startX, startY, endX, endY));

        // Read Ereignisse only after setting the track element coordinates
        if (ereignis == 3036 || ereignis == 3037) {
            // Wendepunkt / Wendepunkt auf anderen Blocknamen
            Wendepunkt *wendepunkt = new Wendepunkt(0, te->line().angle());
            wendepunkt->setPos(te->line().p2());
            m_wendepunkte.append(wendepunkt);
        }

        #ifdef REMOVE_OPPOSITES
        int intStartX = (int)startX;
        if (!trackElementsByStartX.contains(intStartX)) {
            QList<TrackElement*> *newList = new QList<TrackElement*>();
            newList->append(te);
            trackElementsByStartX.insert(intStartX, newList);
        } else {
            trackElementsByStartX[intStartX]->append(te);
        }
        #endif

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
                Signal *sig = new Signal(NULL, te->line().angle(), station + ", " + signal);
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

    // Mark starting points
    foreach (TrackElement *te, startingPoints.keys()) {
        te->setIsStartingPoint(true);

        StartingPoint *startingPoint = new StartingPoint(NULL, te->line().angle(), startingPoints[te]);
        startingPoint->setPos(te->line().p1());
        m_startingPoints.append(startingPoint);

        while (!te->hasSignal() && te->next.size() > 0) {
            te = te->next.front();
            te->setIsStartingPoint(true);
        }
    }

    #ifdef REMOVE_OPPOSITES
    // Remove opposite direction elements
    int originalCount = trackElements.count();
    foreach (TrackElement *te, trackElementsPointingLeft) {
        if (!trackElements.contains(te->number())) {
            continue;
        }

        int oppositeStartX = (int)te->line().p2().x();
        if (trackElementsByStartX.contains(oppositeStartX)) {
            foreach (TrackElement *possibleOpposite, *trackElementsByStartX[oppositeStartX]) {
                if (te->isOppositeOf(possibleOpposite)) {
                    possibleOpposite->deleteFromNeighbors();
                    trackElements.remove(possibleOpposite->number());
                    te->setIsStartingPoint(te->isStartingPoint() || possibleOpposite->isStartingPoint());
                    te->setBothDirections(true);
                    break;
                }
            }
        }
    }

    qDebug() << "Removed" << (originalCount - trackElements.count()) << "elements, now" << trackElements.count();
    qDeleteAll(trackElementsByStartX);
    #endif

    // Create segments from track elements
    foreach (TrackElement *te, trackElements) {
        if (!te->isStartingPointOfSegment()) { continue; }

        // Construct two path items: one for the track segment and one for the segment’s direction arrows,
        // the latter positioned below. This is to prevent overlap on routes which can be used in both direction.
        // The arrows are not shown for tunnel segments because it looks ugly.
        TrackSegment *pathItem = new TrackSegment();
        QGraphicsPathItem *pathArrowsItem = new QGraphicsPathItem();
        QPainterPath path;
        QPainterPath arrowPath;

        pathItem->setZValue(ZVALUE_TRACK);
        pathArrowsItem->setZValue(ZVALUE_ARROWS);

        #ifdef RANDOM_COLORS
        QColor penColor = QColor(qrand() % 256, qrand() % 256, qrand() % 256);
        # else
        QColor penColor = te->isStartingPoint() ? (te->electrified() ? Qt::darkGreen : Qt::green) : (te->electrified() ? Qt::black : Qt::darkGray);
        # endif

        pathItem->setPen(QPen(
                             QBrush(penColor),
                             te->bothDirections() ? 2 : 1,
                             te->tunnel() ? Qt::DotLine : Qt::SolidLine,
                             Qt::FlatCap));

        pathArrowsItem->setPen(QPen(QBrush(penColor), 1, Qt::SolidLine, Qt::FlatCap));

        path.moveTo(te->shiftedLine().p1());
        path.lineTo(te->shiftedLine().p2());
        double pathLength = te->line().length();

        while (te->next.size() > 0 && te->next.front()->prev.front() == te && !te->next.front()->isStartingPointOfSegment()) {
            te = te->next.front();

            path.lineTo(te->shiftedLine().p2());
            pathLength += te->line().length();
        }

        pathItem->setPath(path);
        m_trackSegments.append(pathItem);

        if (!te->tunnel()) {
            // Draw little arrows to indicate the direction of the track element.
            // The arrows are drawn every ARROWEVERY meters, but at least once per track segment.
            // They also are drawn at least ARROWLENGTH m from the beginning of the track segment.
            int numArrows = ((pathLength - ARROWLENGTH) / ARROWEVERY) + 1;
            double firstArrowPos = ARROWLENGTH + (pathLength - ARROWLENGTH - ARROWEVERY * (numArrows - 1)) / 2;

            // Store arrow base points in a map beforehand because drawing them one after another could
            // influence the computation of the following points.
            // We have to store pointers to the points in the map because QPointF does not implement operator<.
            QMap<QPointF*, qreal> arrowBasePoints;
            for (int i = 0; i < numArrows; i++) {

                // Take the maximum of the angles at the arrow base and arrow end to prevent arrows from
                // pointing outside the opposite track segment in curves
                qreal percentArrowBase = std::min(1.0, (firstArrowPos + ARROWEVERY * i) / pathLength);
                qreal percentArrowEnd = std::max(0.0, (firstArrowPos + ARROWEVERY * i - ARROWLENGTH) / pathLength);

                arrowBasePoints.insert(new QPointF(path.pointAtPercent(percentArrowBase)),
                                       std::max(path.angleAtPercent(percentArrowBase), path.angleAtPercent(percentArrowEnd)) / 180 * M_PI);
            }

            foreach (QPointF* arrowBasePoint, arrowBasePoints.keys()) {
                qreal angle = M_PI - arrowBasePoints[arrowBasePoint] + ARROWANGLE;

                arrowPath.moveTo(*arrowBasePoint);
                arrowPath.lineTo(*arrowBasePoint + QPointF(cos(angle) * ARROWLENGTH, sin(angle) * ARROWLENGTH));
            }

            qDeleteAll(arrowBasePoints.keys());
            pathArrowsItem->setPath(arrowPath);
            m_arrows.append(pathArrowsItem);
        }
    }

    qDebug() << m_trackSegments.count() << "track segments";
}

Route::~Route()
{
    qDeleteAll(trackElements);
    qDeleteAll(m_trackSegments);
    qDeleteAll(m_signals);
    qDeleteAll(m_viewPoints);
    qDeleteAll(m_wendepunkte);
    qDeleteAll(m_startingPoints);
}

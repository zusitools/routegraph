#include "model/route.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QHash>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <QGraphicsPolygonItem>
#include <QTime>

#include <cmath>
#include <limits>

#include "signal.h"
#include "zvalues.h"
#include "model/ereignis.h"
#include "model/trackelementsignal.h"

// Uncomment this to remove opposite direction elements
// #define REMOVE_OPPOSITES

/**
 * Skips the specified number of lines in the QTextStream ts.
 */
void skipLine(QTextStream &ts, int count = 1) {
    for (int i = 0; i < count && !ts.atEnd(); i++) {
        ts.readLine();
    }
}

/**
 * Skips lines in the QTextStream ts until the string "terminator" appeared as a whole line the specified number of times.
 */
void skipUntil(QTextStream &ts, QString terminator = "#", int count = 1) {
    while (!ts.atEnd() && count > 0) {
        QString line = ts.readLine();

        if (line == terminator) {
            count--;
        }
    }
}

TrackElement* Route::getTrackElement(int number) {
    TrackElement* result = m_trackElements[number];

    if (result == NULL) {
        result = new TrackElement(number);
        m_trackElements.insert(number, result);
    }

    return result;
}

QList<TrackElement *> Route::trackElementsWithSignals()
{
    QList<TrackElement*> trackElementsWithSignals;
    foreach (TrackElement *te, m_trackElements.values()) {
        if ((te->directionInfo(true) && te->directionInfo(true)->signal) || (te->directionInfo(false) && te->directionInfo(false)->signal)) {
            trackElementsWithSignals.append(te);
        }
    }
    return trackElementsWithSignals;
}

TrackElement *Route::trackElement(const int number)
{
    if (m_trackElements.contains(number)) {
        return m_trackElements.value(number);
    }

    return NULL;
}

QList<QList<FahrstrasseSegment *> *> Route::findRoutesTo(uint32_t startElementNumber, TimetableEntry *target, int recursionDepth)
{
    QList<QList<FahrstrasseSegment *> *> result;
    QList<FahrstrasseSegment*> currentPath;
    currentPath.append(trackElement(startElementNumber)->fahrstrasseSegment());

    findRouteRec(result, currentPath, recursionDepth, target);

    return result;
}

void Route::findRouteRec(QList<QList<FahrstrasseSegment*> *> &results, QList<FahrstrasseSegment*> &currentPath, int recursionDepth, TimetableEntry *target)
{
    TrackElement *last = currentPath.last()->lastElement();
    if (last->directionInfo(true) && last->directionInfo(true)->signal) {
        if (target == NULL || (last->directionInfo(true)->signal->stationName() == target->stationName && target->allowedTracks.contains(last->directionInfo(true)->signal->signalName()))) {
            // We found a path!
            QList<FahrstrasseSegment*> *newPath = new QList<FahrstrasseSegment*>(currentPath);
            results.append(newPath);
            return;
        } else {
            // We found a signal, but it does not match.
            if (--recursionDepth < 0) {
                return;
            }
        }
    }

    if (last->next.size() == 0) {
        return;
    } else {
        // Current element has more than one successor, split the search.
        for (unsigned int i = 0; i < last->next.size(); i++) {
            currentPath.push_back(last->next.at(i)->fahrstrasseSegment());
            findRouteRec(results, currentPath, recursionDepth, target);
            currentPath.pop_back();
        }
    }
}

void Route::setReachableRec(TrackElement *const trackElement) const
{
    TrackElement *te = trackElement;

    // While the track element has only one successor, we do not need to recurse
    while (true) {
        if (te->isReachableFromStartingPoint()) {
            return;
        }

        te->setIsReachableFromStartingPoint(true);

        // Wendepunkt: Opposite direction element is reachable, too
        if (te->directionInfo(true) && (te->directionInfo(true)->hasEreignis(3036) || te->directionInfo(true)->hasEreignis(3037))) {
            TrackElement *opposite = findOppositeTrackElement(te);
            if (opposite != NULL) {
                setReachableRec(opposite);
            }
        }

        if (te->next.size() == 1) {
            te = te->next.front();
        } else {
            break;
        }
    }

    // Recurse into all successors (if there are any).
    for (unsigned int i = 0; i < te->next.size(); i++) {
        setReachableRec(te->next.at(i));
    }
}

TrackElement *Route::findOppositeTrackElement(TrackElement *te) const
{
    int oppositeStartX = (int)te->line().p2().x();

    if (trackElementsByStartX.contains(oppositeStartX)) {
        foreach (TrackElement *possibleOpposite, *trackElementsByStartX[oppositeStartX]) {
            if (te->isOppositeOf(possibleOpposite)) {
                return possibleOpposite;
            }
        }
    }

    return NULL;
}

QString purgeStationName(QString stationName) {
    QString result = stationName;
    int threeDotsIndex = stationName.indexOf("...");

    if (threeDotsIndex != -1) {
        result.truncate(threeDotsIndex);
    }

    return result.replace('|', ' ').trimmed();
}

Route::Route(QString fileName)
{
    QTime myTimer;
    myTimer.start();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    in.setCodec("ISO 8859-1");

    #ifdef REMOVE_OPPOSITES
    QList<TrackElement*> trackElementsPointingLeft;
    #endif

    QHash<TrackElement*, QString> startingPoints;
    QHash<QString, QList<QPointF>*> stations; // position of station labels by station name

    QString version = in.readLine();
    if (version.length() != 3) {
        throw QObject::tr("Unsupported file format");
    } else if (version != "2.2" && version != "2.3") {
        throw QObject::tr("Unsupported file format version: %1").arg(version);
    }

    // Skip header of file
    skipUntil(in, "#", 2);
    skipLine(in);

    m_lsFile = in.readLine();

    // Read starting points
    while (in.readLine() != "#") {
        int number = in.readLine().toInt();
        getTrackElement(number)->setIsStartingPoint(true);
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

    qDebug() << myTimer.elapsed() << "ms for header";
    myTimer.restart();

    // Read track element
    while (!in.atEnd()) {
        int elementNumber = in.readLine().toInt();
        TrackElement *te = getTrackElement(elementNumber);
        te->setZusiVersion(2);
        te->addDirectionInfo(true);
        skipLine(in, 3);

        uint32_t ereignis = in.readLine().toInt();
        if (ereignis != 0) {
            Ereignis *newEreignis = new Ereignis();
            newEreignis->setType(ereignis);
            te->directionInfo(true)->ereignisse.push_back(newEreignis);
        }

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
        if (te->directionInfo(true)->hasEreignis(3036) || te->directionInfo(true)->hasEreignis(3037)) {
            // Wendepunkt / Wendepunkt auf anderen Blocknamen
            Wendepunkt *wendepunkt = new Wendepunkt(0, te->line().angle());
            wendepunkt->setPos(te->line().p2());
            m_wendepunkte.append(wendepunkt);
        }

        int intStartX = (int)startX;
        if (!trackElementsByStartX.contains(intStartX)) {
            QList<TrackElement*> *newList = new QList<TrackElement*>();
            newList->append(te);
            trackElementsByStartX.insert(intStartX, newList);
        } else {
            trackElementsByStartX[intStartX]->append(te);
        }

        #ifdef REMOVE_OPPOSITES
        if (te->line().angle() > 90.0 && te->line().angle() <= 270.0) {
            trackElementsPointingLeft.append(te);
        }
        #endif

        // Read successor elements
        int nextNumber;
        for (int i = 0; i < 3; i++) {
            if ((nextNumber = in.readLine().toInt()) > 0) {
                TrackElement *next = getTrackElement(nextNumber);
                te->next.push_back(next);
                next->prev.push_back(te);
            }
        }

        skipLine(in, 2);

        QString stationName = in.readLine();
        if (te->directionInfo(true)->hasEreignis(3008) /* Bahnsteigmitte */ && !stationName.isEmpty()) {
            stationName = purgeStationName(stationName);

            if (!stations.contains(stationName)) {
                stations.insert(stationName, new QList<QPointF>());
            }

            stations[stationName]->append(QPointF(endX, endY));
        }

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
                te->directionInfo(true)->signal = new TrackElementSignal();
                te->directionInfo(true)->signal->setStationName(station);
                te->directionInfo(true)->signal->setSignalName(signal);
            }

            int numRows = in.readLine().toInt() + 1;
            int numCols = in.readLine().toInt() + 1;

            skipLine(in, 5 * numRows + numCols + 1 + 6 * numRows * numCols + 6 + 1);
            skipUntil(in, "#");
            skipLine(in);
        }

        te->directionInfo(true)->_register = getRegister(in.readLine().toInt());

        m_trackElements.insert(te->number(), te);
    }

    qDebug() << myTimer.elapsed() << "ms for track elements";
    myTimer.restart();

    // Add station names
    foreach (QString stationName, stations.keys()) {
        QPointF center = QPointF(0, 0);

        foreach (QPointF point, *stations[stationName]) {
            center += point;
        }

        center /= stations[stationName]->count();

        Station *station = new Station();
        station->pos = center;
        station->name = stationName;
        m_stations.append(station);
    }

    qDebug() << myTimer.elapsed() << "ms for station names";
    myTimer.restart();

    qDebug() << minX << minY << maxX << maxY;
    qDeleteAll(stations.values());

    file.close();

    // Mark starting points and set "reachable from starting point" value
    foreach (TrackElement *te, startingPoints.keys()) {
        te->setIsStartingSegment(true);
        StartingPoint *startingPoint = new StartingPoint(NULL, te->line().angle(), startingPoints[te]);
        startingPoint->setPos(te->line().p1());
        m_startingPoints.append(startingPoint);

        setReachableRec(te);

        while (te->directionInfo(true) && !te->directionInfo(true)->signal && te->next.size() > 0) {
            te = te->next.front();
            te->setIsStartingSegment(true);
        }
    }

    qDebug() << myTimer.elapsed() << "ms for starting points and reachability";
    myTimer.restart();

    #ifdef REMOVE_OPPOSITES
    // Remove opposite direction elements
    int originalCount = m_trackElements.count();
    foreach (TrackElement *te, trackElementsPointingLeft) {
        if (!m_trackElements.contains(te->number())) {
            continue;
        }

        TrackElement *opposite = findOppositeTrackElement(te);

        if (opposite != NULL) {
            opposite->deleteFromNeighbors();
            m_trackElements.remove(opposite->number());
            te->setIsStartingPoint(te->isStartingPoint() || opposite->isStartingPoint());
            te->setBothDirections(true);
        }
    }

    qDebug() << "Removed" << (originalCount - m_trackElements.count()) << "elements, now" << m_trackElements.count();
    qDebug() << myTimer.elapsed() << "ms for removing opposites";
    myTimer.restart();
    #endif

    qDeleteAll(trackElementsByStartX);

    // Fahrstrasse segments
    foreach (TrackElement *te, m_trackElements) {
        if (!te->isStartingPointOfFahrstrasseSegment()) { continue; }

        FahrstrasseSegment *segment = new FahrstrasseSegment();
        m_fahrstrasseSegments << segment;

        segment->trackElements()->append(te);
        te->setFahrstrasseSegment(segment);

        while (te->next.size() == 1) {
            te = te->next.front();

            if (te->isStartingPointOfFahrstrasseSegment()) {
                break;
            } else {
                segment->trackElements()->append(te);
                te->setFahrstrasseSegment(segment);
            }
        }
    }

    qDebug() << myTimer.elapsed() << "ms for Fahrstrasse segments";
    myTimer.restart();

    qDebug() << m_fahrstrasseSegments.count() << "Fahrstrasse segments";
}

Route::~Route()
{
    qDeleteAll(m_trackElements);
    qDeleteAll(m_viewPoints);
    qDeleteAll(m_wendepunkte);
    qDeleteAll(m_startingPoints);
    qDeleteAll(m_fahrstrasseSegments);
    qDeleteAll(m_stations);
    qDeleteAll(m_registers);
}

Register *Route::getRegister(int regNumber)
{
    if (regNumber == 0)
    {
        return NULL;
    }
    if (!m_registers.contains(regNumber))
    {
        m_registers.insert(regNumber, new Register(regNumber));
    }
    return m_registers.value(regNumber);
}

void Route::setOccupiedRegisters(QSet<int> *occupiedRegisters)
{
    foreach(Register* reg, this->getRegisters())
    {
        reg->setOccupationState((occupiedRegisters->contains(reg->getRegNumber())) ? Register::Occupied : Register::Free);
    }
}

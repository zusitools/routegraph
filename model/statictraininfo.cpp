#ifndef STATICTRAININFO_CPP
#define STATICTRAININFO_CPP

#include <QDebug>

#include "model/statictraininfo.h"

QDataStream &operator <<(QDataStream &stream, const StaticTrainInfo *staticTrainInfo)
{
    stream << staticTrainInfo->address << staticTrainInfo->trainType << staticTrainInfo->trainNumber;

    // Write occupied track elements/Fahrstrasse segments
    QList<uint32_t> occupiedTrackElementNumbers;
    foreach (TrackElement *te, staticTrainInfo->occupiedTrackElements) {
        occupiedTrackElementNumbers.append(te->number());
    }

    QList<uint32_t> occupiedFahrstrasseSegmentStartNumbers;
    foreach (FahrstrasseSegment *fs, staticTrainInfo->occupiedFahrstrasseSegments) {
        occupiedFahrstrasseSegmentStartNumbers.append(fs->firstElement()->number());
    }

    stream << occupiedTrackElementNumbers << occupiedFahrstrasseSegmentStartNumbers;

    return stream;
}

QDataStream &operator >>(QDataStream &stream, StaticTrainInfo*& staticTrainInfo)
{
    staticTrainInfo = new StaticTrainInfo();
    stream >> staticTrainInfo->address >> staticTrainInfo->trainType >> staticTrainInfo->trainNumber
              >> staticTrainInfo->occupiedTrackElementNumbers >> staticTrainInfo->occupiedFahrstrasseSegmentStartNumbers;

    return stream;
}

#endif // STATICTRAININFO_CPP

#ifndef UTILS_CPP
#define UTILS_CPP

#include "utils.h"
#include "time.h"
#include <QDebug>

QDateTime timeToDateTime(time_t value)
{
    // HACKY HACK: Try to account for daylight saving time (DST) in the supplied value.
    time_t time = value;
    tm *tmval;

#ifdef _WIN32
    tm localtm;
    errno_t err = localtime_s(&localtm, &time);

    if (err) {
        qDebug() << "sim time error: " << err;
    } else {
        tmval = &localtm;
#else
    {
        tmval = localtime(&time);
#endif
        tmval->tm_isdst = -1; // attempt to find out whether DST applies
        mktime(tmval);
    }

    QDateTime dateTime;
    dateTime.setTime_t(value - 3600 * tmval->tm_isdst);
    return dateTime;
}

#endif // UTILS_CPP

#-------------------------------------------------
#
# Project created by QtCreator 2013-01-03T15:31:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = routegraph
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    trackview.cpp \
    memread/zusimemoryreader.cpp \
    memread/zusimemoryreaderlinux.cpp \
    memread/zusimemoryreaderexception.cpp \
    trainmanager.cpp \
    graphicsitems/fahrstrassesegmentitem.cpp \
    model/fahrstrassesegment.cpp \
    model/register.cpp \
    graphicsitems/minwidthpathitem.cpp \
    model/train.cpp \
    graphicsitems/marker.cpp \
    graphicsitems/signalmarker.cpp \
    graphicsitems/startingpoint.cpp \
    model/trackelement.cpp \
    graphicsitems/tracksegment.cpp \
    graphicsitems/wendepunkt.cpp \
    graphicsitems/viewpoint.cpp \
    graphicsitems/label.cpp \
    model/route.cpp \
    graphicsitems/trainitem.cpp \
    graphicsitems/fahrstrasseaufloesenmarker.cpp \
    model/statictraininfo.cpp \
    libqxt/qxtspanslider.cpp \
    libqxt/qxtglobal.cpp \
    savecapturedialog.cpp \
    utils.cpp \
    graphicsitems/registermarker.cpp \
    model/ereignis.cpp \
    model/trackelementsignal.cpp

HEADERS  += mainwindow.h \
    trackview.h \
    zvalues.h \
    memread/zusimemoryreader.h \
    memread/zusimemoryreaderlinux.h \
    memread/zusimemoryreaderexception.h \
    trainmanager.h \
    graphicsitems/fahrstrassesegmentitem.h \
    model/fahrstrassesegment.h \
    model/register.h \
    model/statictraininfo.h \
    graphicsitems/minwidthpathitem.h \
    model/station.h \
    shareddeque.h \
    model/timetableentry.h \
    graphicsitems/label.h \
    graphicsitems/marker.h \
    graphicsitems/signalmarker.h \
    graphicsitems/wendepunkt.h \
    graphicsitems/viewpoint.h \
    graphicsitems/trainitem.h \
    model/train.h \
    model/trackelement.h \
    graphicsitems/tracksegment.h \
    graphicsitems/startingpoint.h \
    graphicsitems/fahrstrasseaufloesenmarker.h \
    model/route.h \
    libqxt/qxtspanslider_p.h \
    libqxt/qxtspanslider.h \
    libqxt/qxtglobal.h \
    savecapturedialog.h \
    utils.h \
    graphicsitems/registermarker.h \
    model/ereignis.h \
    model/trackelementsignal.h

FORMS    += mainwindow.ui \
    savecapturedialog.ui

TRANSLATIONS = routegraph_de.ts

# -Weffc++ for checking for uninitialized data members
QMAKE_CXXFLAGS = -std=c++11

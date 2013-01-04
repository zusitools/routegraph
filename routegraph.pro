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
    route.cpp \
    trackelement.cpp \
    trackview.cpp \
    viewpoint.cpp \
    label.cpp \
    signal.cpp

HEADERS  += mainwindow.h \
    route.h \
    trackelement.h \
    trackview.h \
    viewpoint.h \
    label.h \
    signal.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS = -std=c++11

#-------------------------------------------------
#
# Project created by QtCreator 2017-04-23T11:26:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtIrrlicht4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tinyply.cpp

HEADERS  += mainwindow.h \
    irrlichtwidget.h \
    myeventreceiver.h \
    gizmo.h \
    irrlichtwrap.h \
    tinyply.h \
    object.h \
    nanoflann.hpp

FORMS    += mainwindow.ui


INCLUDEPATH += /usr/include/irrlicht
LIBS += -l Irrlicht

unix:!macx: LIBS += -L$$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/lib/ -lreactphysics3d

INCLUDEPATH += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/src
DEPENDPATH += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/lib/libreactphysics3d.a

DISTFILES += \
    frag.glsl \
    vert.vsh


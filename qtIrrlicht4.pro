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
        mainwindow.cpp

HEADERS  += mainwindow.h \
    irrlichtwidget.h \
    myeventreceiver.h \
    gizmo.h \
    irrlichtbullet.h

FORMS    += mainwindow.ui


INCLUDEPATH += /usr/include/irrlicht
LIBS += -l Irrlicht

unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lBulletCollision
unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lBulletDynamics
unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lBulletSoftBody
unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lLinearMath

INCLUDEPATH += $$PWD/../../../usr/include/bullet
DEPENDPATH += $$PWD/../../../usr/include/bullet

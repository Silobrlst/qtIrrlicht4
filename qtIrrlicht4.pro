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
    tinyply.cpp \
    GizmoTransformMove.cpp \
    GizmoTransformRender.cpp \
    GizmoTransformRotate.cpp \
    GizmoTransformScale.cpp \
    test.cpp \
    ZBaseMaths.cpp \
    ZBaseMaths.inl \
    ZMathsFunc.cpp

HEADERS  += mainwindow.h \
    irrlichtwidget.h \
    myeventreceiver.h \
    irrlichtwrap.h \
    tinyply.h \
    object.h \
    nanoflann.hpp \
    GizmoTransform.h \
    GizmoTransformMove.h \
    GizmoTransformRender.h \
    GizmoTransformRotate.h \
    GizmoTransformScale.h \
    IGizmo.h \
    LibBase.h \
    test.h \
    ZBaseDefs.h \
    ZBaseMaths.h \
    ZCollisionsUtils.h \
    ZMathsFunc.h \
    irrlichtwidget.h.autosave \
    attributetreedelegate.h

FORMS    += mainwindow.ui


INCLUDEPATH += /usr/include/irrlicht
LIBS += -l Irrlicht

unix:!macx: LIBS += -L$$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/lib/ -lreactphysics3d

INCLUDEPATH += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/src
DEPENDPATH += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/src

unix:!macx: PRE_TARGETDEPS += $$PWD/../Загрузки/reactphysics3d-0.6.0/reactphysics3d/lib/libreactphysics3d.a

DISTFILES += \
    frag.glsl \
    vert.vsh \
    qtIrrlicht4.pro.user \
    README.md \
    closed_hover.png \
    closed.png \
    cursor.png \
    open_hover.png \
    open.png \
    select.png \
    select2.png \
    transform_move.png \
    transform_rotate.png \
    transform_scale.png


#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include <irrlicht.h>
#include <iostream>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

class test : public QWidget
{
    Q_OBJECT

    IrrlichtDevice *m_device;
public:
    explicit test(QWidget *parent){
        irr::SIrrlichtCreationParameters params;
        params.DriverType = irr::video::EDT_OPENGL;
        params.WindowId = (void*)winId();
        params.WindowSize.Width = parent->size().width();
        params.WindowSize.Height = parent->size().height();
        m_device = irr::createDeviceEx(params);
    }

signals:

public slots:
};

#endif // TEST_H

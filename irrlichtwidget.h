#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include <irrlicht.h>
#include <iostream>

#include <gizmo.h>
#include "myeventreceiver.h"

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;


class IrrlichtWidget : public QWidget{
    Q_OBJECT

public:
    QLineEdit *rText;
    QLineEdit *gText;
    QLineEdit *bText;

    IrrlichtWidget(QWidget *parent){
        isCreateSphere = false;
        rText = 0;
        gText = 0;
        bText = 0;

        setMouseTracking(true);

        this->setFixedSize(640, 480);

        MyEventReceiver *recv = new MyEventReceiver();

        irr::SIrrlichtCreationParameters params;
        params.DriverType = irr::video::EDT_OPENGL;
        params.WindowId = (void*)winId();
        params.WindowSize.Width = parent->size().width();
        params.WindowSize.Height = parent->size().height();
        params.EventReceiver = recv;
        m_device = irr::createDeviceEx(params);
        recv->setDevice(m_device);
        video::IVideoDriver *driver = m_device->getVideoDriver();
        scene::ISceneManager *smgr = m_device->getSceneManager();
        smgr->getFileSystem()->changeWorkingDirectoryTo(".");

        car = smgr->addAnimatedMeshSceneNode(smgr->getMesh("Futuristic_Car_2.1_obj.obj"));
        car->setScale(vector3df(10, 10, 10));
        car->setMaterialTexture(0, driver->getTexture("textures/Futuristic_Car_C.jpg"));
        car->setMaterialFlag( video::EMF_LIGHTING, false );

        //<gizmo>------------------------------------------
        gizmo = new Gizmo(smgr);
        //</gizmo>------------------------------------------

        ICameraSceneNode *cam = smgr->addCameraSceneNodeFPS();
        cam->setInputReceiverEnabled(false);
        cam->setTarget(vector3df(0, 0, 0));
        cam->setPosition(vector3df(40, 20, 50));

        m_timer = new QTimer;
        m_timer->setInterval(0);
        QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(loop()));
        m_timer->start();
    }

    irr::IrrlichtDevice *getIrrlichtDevice() const{
        return m_device;
    }
public slots:
    void loop(){
        if(m_device->run()){
            video::IVideoDriver *driver = m_device->getVideoDriver();
            scene::ISceneManager *smgr = m_device->getSceneManager();
            scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();

            driver->beginScene(true, true, video::SColor(255,100,0,255));

            SMaterial material = SMaterial();
            material.EmissiveColor = SColor(255, 100, 100, 100);
            driver->setMaterial(material);
            driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

            vector3df camPos = smgr->getActiveCamera()->getPosition();
            gizmo->update(camPos, car->getPosition());

            float linesScale = 5;
            for(int i=-5; i<=5; i++){
                if(i == 0){
                    i++;
                }
                driver->draw3DLine(vector3df(i*linesScale, 0, -5*linesScale), vector3df(i*linesScale, 0, 5*linesScale), SColor(255, 100, 100, 100));
            }
            for(int i=-5; i<=5; i++){
                if(i == 0){
                    i++;
                }
                driver->draw3DLine(vector3df(-5*linesScale, 0, i*linesScale), vector3df(5*linesScale, 0, i*linesScale), SColor(255, 100, 100, 100));
            }

            float maxLength = 99999;

            material.EmissiveColor = SColor(255, 255, 0, 0);
            driver->setMaterial(material);
            driver->draw3DLine(vector3df(-maxLength, 0, 0), vector3df(maxLength, 0, 0), SColor(255, 255, 0, 0));

            material.EmissiveColor = SColor(255, 0, 255, 0);
            driver->setMaterial(material);
            driver->draw3DLine(vector3df(0, -maxLength, 0), vector3df(0, maxLength, 0), SColor(255, 0, 255, 0));

            material.EmissiveColor = SColor(255, 0, 0, 255);
            driver->setMaterial(material);
            driver->draw3DLine(vector3df(0, 0, -maxLength), vector3df(0, 0, maxLength), SColor(255, 0, 0, 255));

            if(isCreateSphere){
                vector2d<s32> mousePos = m_device->getCursorControl()->getPosition();
                line3d<f32> ray = collMan->getRayFromScreenCoordinates(mousePos, smgr->getActiveCamera());

                vector3df pos = camPos + ray.getVector().normalize()*30;
                sphere->setPosition(pos);
            }

            smgr->drawAll();

            driver->clearZBuffer();

            gizmo->render();

            driver->endScene();
        }
    }

    void createSphere(){
        isCreateSphere = true;
        sphere = m_device->getSceneManager()->addSphereSceneNode();
        if(rText != 0 && gText != 0 && bText != 0){
            sphere->getMaterial(0).EmissiveColor.set(255, rText->text().toInt(), gText->text().toInt(), bText->text().toInt());
        }
    }

protected:
    void keyPressEvent(QKeyEvent *event){
        irr::SEvent irrEvent;
        irrEvent.EventType = irr::EET_KEY_INPUT_EVENT;
        IrrlichtKey irrKey;
        irrKey.code = (irr::EKEY_CODE)(0);
        irrKey.ch = (wchar_t)(0);
        if((event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) || (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)){
            irrKey.code = (irr::EKEY_CODE)event->key();
            irrKey.ch = (wchar_t)event->key();
        }
        else{
            switch(event->key()){
            case Qt::Key_Up:
                irrKey.code = irr::KEY_UP;
                break;

            case Qt::Key_Down:
                irrKey.code = irr::KEY_DOWN;
                break;

            case Qt::Key_Left:
                irrKey.code = irr::KEY_LEFT;
                break;

            case Qt::Key_Right:
                irrKey.code = irr::KEY_RIGHT;
                break;

            case Qt::Key_Escape:
                irrKey.code = irr::KEY_ESCAPE;
                break;

            case Qt::Key_Space:
                irrKey.code = irr::KEY_SPACE;
                break;
            }
        }
        if(irrKey.code != 0){
            irrEvent.KeyInput.Key = irrKey.code;
            irrEvent.KeyInput.Control = ((event->modifiers() & Qt::ControlModifier) != 0);
            irrEvent.KeyInput.Shift = ((event->modifiers() & Qt::ShiftModifier) != 0);
            irrEvent.KeyInput.Char = irrKey.ch;
            irrEvent.KeyInput.PressedDown = true;

            m_device->postEventFromUser(irrEvent);
        }
        event->ignore();
    }

    void keyReleaseEvent(QKeyEvent *event){
        irr::SEvent irrEvent;
        irrEvent.EventType = irr::EET_KEY_INPUT_EVENT;
        IrrlichtKey irrKey;
        irrKey.code = (irr::EKEY_CODE)(0);
        irrKey.ch = (wchar_t)(0);
        if((event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) || (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)){
            irrKey.code = (irr::EKEY_CODE)event->key();
            irrKey.ch = (wchar_t)event->key();
        }
        else{
            switch(event->key()){
            case Qt::Key_Up:
                irrKey.code = irr::KEY_UP;
                break;

            case Qt::Key_Down:
                irrKey.code = irr::KEY_DOWN;
                break;

            case Qt::Key_Left:
                irrKey.code = irr::KEY_LEFT;
                break;

            case Qt::Key_Right:
                irrKey.code = irr::KEY_RIGHT;
                break;

            case Qt::Key_Escape:
                irrKey.code = irr::KEY_ESCAPE;
                break;

            case Qt::Key_Space:
                irrKey.code = irr::KEY_SPACE;
                break;
            }
        }
        if(irrKey.code != 0){
            irrEvent.KeyInput.Key = irrKey.code;
            irrEvent.KeyInput.Control = ((event->modifiers() & Qt::ControlModifier) != 0);
            irrEvent.KeyInput.Shift = ((event->modifiers() & Qt::ShiftModifier) != 0);
            irrEvent.KeyInput.Char = irrKey.ch;
            irrEvent.KeyInput.PressedDown = false;

            m_device->postEventFromUser(irrEvent);
        }
        event->ignore();
    }

    void mousePressEvent(QMouseEvent *event){
        irr::SEvent irrEvent;

        irrEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

        switch(event->button()){
        case Qt::LeftButton:
            irrEvent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
            break;

        case Qt::MidButton:
            irrEvent.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
            break;

        case Qt::RightButton:
            irrEvent.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
            break;

        default:
            return;
        }

        irrEvent.MouseInput.X = event->x();
        irrEvent.MouseInput.Y = event->y();
        irrEvent.MouseInput.Wheel = 0.0f;

        m_device->postEventFromUser(irrEvent);
        event->ignore();
    }

    void mouseReleaseEvent(QMouseEvent* event){
        irr::SEvent irrEvent;

        irrEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

        switch(event->button()){
        case Qt::LeftButton:
            irrEvent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;

            if(isCreateSphere){
                isCreateSphere = false;
            }

            break;

        case Qt::MidButton:
            irrEvent.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
            break;

        case Qt::RightButton:
            irrEvent.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
            break;

        default:
            return;
        }

        irrEvent.MouseInput.X = event->x();
        irrEvent.MouseInput.Y = event->y();
        irrEvent.MouseInput.Wheel = 0.0f;

        m_device->postEventFromUser(irrEvent);
        event->ignore();
    }

    void wheelEvent(QWheelEvent* event){
        if(event->orientation() == Qt::Vertical){
            irr::SEvent irrEvent;

            irrEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

            irrEvent.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
            irrEvent.MouseInput.X = 0;
            irrEvent.MouseInput.Y = 0;
            irrEvent.MouseInput.Wheel = event->delta() / 120.0f;

            m_device->postEventFromUser(irrEvent);
        }
        event->ignore();
    }

    void mouseMoveEvent( QMouseEvent* event ){
        irr::SEvent irrEvent;
        irrEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
        if ( m_device != 0 ){
            irrEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
            irrEvent.MouseInput.X = event->x();
            irrEvent.MouseInput.Y = event->y();
            irrEvent.MouseInput.Wheel = 0.0f; // Zero is better than undefined
            if(m_device->postEventFromUser( irrEvent ))
                event->accept();
        }
    }

private:
    struct IrrlichtKey{
        irr::EKEY_CODE code;
        wchar_t ch;
    };

    Gizmo *gizmo;

    scene::ISceneNode *car;

    bool isCreateSphere;
    ISceneNode *sphere;

    irr::IrrlichtDevice *m_device;
    QTimer *m_timer;
};

#endif // WIDGET_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include <QRegExp>
#include <qlabel.h>
#include <QCheckBox>
#include <QTreeWidgetItem>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>

#include <irrlicht.h>

#include "IGizmo.h"
#include "myeventreceiver.h"

#include <irrlicht.h>
#include <irrlichtwrap.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

enum ToolStatus{
    ToolNothing,
    ToolMove,
    ToolSphere,
    ToolPointCloud,
    ToolLineFrom,
    ToolLineTo,
    ToolMoveFromCamera
};

class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:

    virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData){
        video::IVideoDriver* driver = services->getVideoDriver();

        core::matrix4 worldViewProj;
        worldViewProj = driver->getTransform(video::ETS_PROJECTION);
        worldViewProj *= driver->getTransform(video::ETS_VIEW);
        worldViewProj *= driver->getTransform(video::ETS_WORLD);
        services->setVertexShaderConstant("_mvProj", worldViewProj.pointer(), 16);
    }
};

class IrrlichtWidget : public QWidget{
    Q_OBJECT

public:
    QLineEdit *moveFromCameraDist;
    QCheckBox *moveFromCameraInverse;

    QLabel *statusText;

    const QString dataFileName = "data.json";

    IrrlichtWidget(QWidget *parent){
        srand( time( 0 ) );

        toolStatus = ToolNothing;
        rightMouseButtonDown = false;
        leftMouseButtonDown = false;

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
        IVideoDriver* driver = m_device->getVideoDriver();
        smgr = m_device->getSceneManager();
        smgr->getFileSystem()->changeWorkingDirectoryTo(".");

        selectedObj = 0;
        hitedObj = 0;

        core = new Core(smgr);
        core->fromJSONfile(dataFileName);
        core->setAllLinesWidth(10.);

        ICameraSceneNode *cam = smgr->addCameraSceneNodeFPS();
        cam->setInputReceiverEnabled(false);
        cam->setTarget(vector3df(0, 0, 0));
        cam->setPosition(vector3df(30, 30, 30));

        //        std::string psFileName = "/home/q/qtIrrlicht4/frag.glsl";
        //        std::string vsFileName = "/home/q/qtIrrlicht4/vert.glsl";
        //        MyShaderCallBack* mc = new MyShaderCallBack();
        //        materialType = gpu->addHighLevelShaderMaterialFromFiles(vsFileName.c_str(), "main", video::EVST_VS_1_1, psFileName.c_str(), "main", video::EPST_PS_1_1, mc);
        //        mc->drop();

        gizmoMove = CreateMoveGizmo();
        gizmoRotate = CreateRotateGizmo();
        gizmoScale = CreateScaleGizmo();

        gizmo = gizmoMove;
        gizmo->SetLocation( IGizmo::LOCATE_WORLD );
        gizmo->SetScreenDimension( 640, 480 );
        gizmoMove->SetDisplayScale( 2.f );
        gizmoRotate->SetDisplayScale( 2.f );
        gizmoScale->SetDisplayScale( 2.f );


        m_timer = new QTimer;
        m_timer->setInterval(0);
        QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(loop()));
        m_timer->start();
    }

    irr::IrrlichtDevice *getIrrlichtDevice() const{
        return m_device;
    }

    void renderAxisesAndGrid(){
        video::IVideoDriver *driver = m_device->getVideoDriver();
        ICameraSceneNode *cam = m_device->getSceneManager()->getActiveCamera();
        vector3df camPos = cam->getPosition();
        float maxLength = cam->getFarValue();

        SMaterial material;
        material.EmissiveColor = SColor(255, 100, 100, 100);
        driver->setMaterial(material);
        driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
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

        material.EmissiveColor = SColor(255, 255, 0, 0);
        driver->setMaterial(material);
        driver->draw3DLine(vector3df(-maxLength+camPos.X, 0, 0), vector3df(maxLength+camPos.X, 0, 0), SColor(255, 255, 0, 0));

        material.EmissiveColor = SColor(255, 0, 255, 0);
        driver->setMaterial(material);
        driver->draw3DLine(vector3df(0, -maxLength+camPos.Y, 0), vector3df(0, maxLength+camPos.Y, 0), SColor(255, 0, 255, 0));

        material.EmissiveColor = SColor(255, 0, 0, 255);
        driver->setMaterial(material);
        driver->draw3DLine(vector3df(0, 0, -maxLength+camPos.Z), vector3df(0, 0, maxLength+camPos.Z), SColor(255, 0, 0, 255));
    }

    void selectObject(Object *objectIn){
        selectedObj = objectIn;
    }

    void unselect(){
        if(selectedObj != 0){
            selectedObj = 0;
        }
    }

    void moveSphereAroundCamera(TransformableObject *objectIn, float distanceIn=30){
        scene::ISceneManager *smgr = m_device->getSceneManager();
        scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
        vector3df camPos = smgr->getActiveCamera()->getPosition();
        vector2d<s32> mousePos = m_device->getCursorControl()->getPosition();

        line3d<f32> ray = collMan->getRayFromScreenCoordinates(mousePos, smgr->getActiveCamera());
        vector3df pos = camPos + ray.getVector().normalize()*objectIn->getScale()*distanceIn;

        if(distanceIn == 0){
            pos = camPos + ray.getVector().normalize()*camPos.getDistanceFrom(objectIn->getPosition());
        }else{
            pos = camPos + ray.getVector().normalize()*(objectIn->getScale() + distanceIn);
        }

        objectIn->setPosition(pos);
    }

public slots:
    void loop(){
        if(m_device->run()){
            video::IVideoDriver *driver = m_device->getVideoDriver();
            scene::ISceneManager *smgr = m_device->getSceneManager();
            scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
            ICameraSceneNode *cam = smgr->getActiveCamera();
            vector3df camPos = cam->getPosition();
            vector2d<s32> mousePos = m_device->getCursorControl()->getPosition();

            driver->beginScene(true, true, video::SColor(255,50,50,100));
            renderAxisesAndGrid();

            core->unselectAll();
            if(hitedObj == 0){
                this->setCursor(Qt::ArrowCursor);
            }else{
                hitedObj->setSelected(true);

                if(hitedObj != selectedObj){
                    this->setCursor(Qt::CrossCursor);
                }else{
                    this->setCursor(Qt::ArrowCursor);
                }
            }

            if(selectedObj != 0){
                selectedObj->setSelected(true);
            }

            //            if(gizmo->grabed && selectedObj != 0){
            //                toTextAllParameters(selectedObj);
            //            }

            if(toolStatus == ToolMoveFromCamera){
                if(rightMouseButtonDown){
                    TransformableObject *obj = (TransformableObject*)selectedObj;
                    moveSphereAroundCamera(obj, 0);
                }
                if(leftMouseButtonDown){
                    TransformableObject *obj = (TransformableObject*)selectedObj;
                    float dist = leftButtonInitmousePos.getDistanceFrom(mousePos);

                    if(moveFromCameraInverse->isChecked()){
                        dist *= -1;
                    }

                    vector3df pos = initPos + (initPos - camPos).normalize()*dist;

                    obj->setPosition(pos);

                    moveFromCameraDist->setText(QString::number(camPos.getDistanceFrom(pos)));

                    driver->draw2DLine(leftButtonInitmousePos, mousePos);
                }
            }

            hitedObj = core->getObjectUnderCursor(mousePos);

            core->renderAll();
            smgr->drawAll();

            gizmo->SetCameraMatrix( cam->getViewMatrix().pointer(), cam->getProjectionMatrix().pointer() );

            if(toolStatus == ToolMove && selectedObj != 0){
                gizmo->SetEditMatrix( ((TransformableObject*)selectedObj)->getMatrix()->pointer() );
                gizmo->Draw();
            }

            driver->endScene();
        }
    }

    void selectTool(){
        toolStatus = ToolNothing;
    }

    void moveTool(){
        toolStatus = ToolMove;
    }

    void moveFromCameraTool(){
        toolStatus = ToolMoveFromCamera;
    }

    void applyFromCameraDist(){
        vector3df camPos = smgr->getActiveCamera()->getPosition();
        Sphere *sph = (Sphere*)selectedObj;
        float dist = moveFromCameraDist->text().toFloat();
        vector3df pos = camPos + (sph->getPosition() - camPos).normalize()*dist;

        sph->setPosition(pos);
    }

    void saveData(){
        core->toJSONfile(dataFileName);
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
            gizmo->OnMouseDown( 640-event->pos().x(), 480-event->pos().y());

            leftMouseButtonDown = true;
            leftButtonInitmousePos = m_device->getCursorControl()->getPosition();
            irrEvent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
            break;

        case Qt::MidButton:
            irrEvent.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
            break;

        case Qt::RightButton:
            rightMouseButtonDown = true;
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
            gizmo->OnMouseUp( 640-event->pos().x(), 480-event->pos().y() );

            irrEvent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
            leftMouseButtonDown = false;

            //            if((!gizmo->grabed || toolStatus == ToolNothing) && !(toolStatus == ToolMoveFromCamera)){
            //                if(hitedObj == 0){
            //                    unselect();
            //                }

            //                selectObject(hitedObj);
            //            }

            if((toolStatus == ToolNothing) && !(toolStatus == ToolMoveFromCamera)){
                if(hitedObj == 0){
                    unselect();
                }

                selectObject(hitedObj);
            }

            break;

        case Qt::MidButton:
            irrEvent.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
            break;

        case Qt::RightButton:
            rightMouseButtonDown = false;
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
            gizmo->OnMouseMove( 640-event->x(), 480-event->y() );
            //gizmo->mouseMoveEvent(vector2d<s32>(event->x(), event->y()));
            irrEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
            irrEvent.MouseInput.X = event->x();
            irrEvent.MouseInput.Y = event->y();
            irrEvent.MouseInput.Wheel = 0.0f; // Zero is better than undefined
            if(m_device->postEventFromUser( irrEvent ))
                event->accept();
        }
    }

private:
    bool checkFloat(QString textIn){
        QRegExp re( "-?\\d{1,}\\.?\\d*" );
        return re.exactMatch(textIn);
    }

    bool checkInteger(QString textIn){
        QRegExp re( "-?\\d{1,}" );
        return re.exactMatch(textIn);
    }

    float getlineC(float x1In, float y1In, float x2In, float y2In){
        float k = (y2In - y1In)/(x2In - x1In);
        return x1In - k*y1In;
    }

    struct IrrlichtKey{
        irr::EKEY_CODE code;
        wchar_t ch;
    };

    Core *core;

    s32 shaderType;

    //toolStatus определяет какой инструмент выбран
    ToolStatus toolStatus;
    bool leftMouseButtonDown;
    bool rightMouseButtonDown;
    vector2d<s32> leftButtonInitmousePos;
    vector3df initPos;

    Object *selectedObj;
    Object *hitedObj;

    irr::IrrlichtDevice *m_device;
    ISceneManager *smgr;
    QTimer *m_timer;

    IGizmo *gizmo;
    IGizmo *gizmoMove, *gizmoRotate, *gizmoScale;
    matrix4 viewInv;
    matrix4 projInv;
};

#endif // WIDGET_H

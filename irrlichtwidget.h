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

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>

#include <irrlicht.h>

#include <gizmo.h>
#include "myeventreceiver.h"

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
    //<node panel>---------------------------------
    QLineEdit *xText;
    QLineEdit *yText;
    QLineEdit *zText;

    QLineEdit *rText;
    QLineEdit *gText;
    QLineEdit *bText;
    QLineEdit *nodeRGBWeightText;

    QLineEdit *nameText;
    QLineEdit *radiusText;
    //</node panel>--------------------------------

    //<edge panel>---------------------------------
    QLineEdit *edgeNameText;

    QLineEdit *edgeRtext;
    QLineEdit *edgeGtext;
    QLineEdit *edgeBtext;

    QLineEdit *edgeDistText;
    QLineEdit *edgeDistMaxText;
    QLineEdit *edgeDistMinText;
    QLineEdit *edgeDistWeightText;

    QLineEdit *edgeRGBdiffText;
    QLineEdit *edgeRGBdiffMaxText;
    QLineEdit *edgeRGBdiffMinText;
    QLineEdit *edgeRGBdiffWeightText;

    QLineEdit *edgeXdiffText;
    QLineEdit *edgeXmaxText;
    QLineEdit *edgeXminText;
    QLineEdit *edgeXweightText;

    QLineEdit *edgeYdiffText;
    QLineEdit *edgeYmaxText;
    QLineEdit *edgeYminText;
    QLineEdit *edgeYweightText;

    QLineEdit *edgeZdiffText;
    QLineEdit *edgeZmaxText;
    QLineEdit *edgeZminText;
    QLineEdit *edgeZweightText;
    //</edge panel>--------------------------------

    QLineEdit *moveFromCameraDist;
    QCheckBox *moveFromCameraInverse;

    QLineEdit *pointCloudRadius;
    QLineEdit *pointCloudNum;
    QLineEdit *pointCloudPointSize;
    QCheckBox *pointCloudRandomRGB;

    QLabel *statusText;

    const QString dataFileName = "data.json";

    IrrlichtWidget(QWidget *parent){
        srand( time( 0 ) );

        toolStatus = ToolNothing;
        rText = 0;
        gText = 0;
        bText = 0;
        sphereNum = 0;
        lineNum = 0;
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

        gizmo = new Gizmo(smgr);

        ICameraSceneNode *cam = smgr->addCameraSceneNodeFPS();
        cam->setInputReceiverEnabled(false);
        cam->setTarget(vector3df(0, 0, 0));
        cam->setPosition(vector3df(30, 30, 30));

        //        std::string psFileName = "/home/q/qtIrrlicht4/frag.glsl";
        //        std::string vsFileName = "/home/q/qtIrrlicht4/vert.glsl";
        //        MyShaderCallBack* mc = new MyShaderCallBack();
        //        materialType = gpu->addHighLevelShaderMaterialFromFiles(vsFileName.c_str(), "main", video::EVST_VS_1_1, psFileName.c_str(), "main", video::EPST_PS_1_1, mc);
        //        mc->drop();


        m_timer = new QTimer;
        m_timer->setInterval(0);
        QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(loop()));
        m_timer->start();

        pc = new PointCloud(smgr, "/home/q/Загрузки/vienna_parliament_relief.ply");
        pc->setScale(100.);
    }

    irr::IrrlichtDevice *getIrrlichtDevice() const{
        return m_device;
    }

    void toTextAllParameters(Object *objectIn){
        clearAllText();

        if(objectIn != 0){
            if(objectIn->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)objectIn;
                SColor color = sph->getColor();
                vector3df pos = sph->getPosition();

                xText->setText(QString::number(pos.X));
                yText->setText(QString::number(pos.Y));
                zText->setText(QString::number(pos.Z));

                rText->setText(QString::number(color.getRed()));
                gText->setText(QString::number(color.getGreen()));
                bText->setText(QString::number(color.getBlue()));
                nodeRGBWeightText->setText(QString::number(sph->rgbWeight));

                nameText->setText(sph->getName());
                radiusText->setText(QString::number(sph->getScale()));
            }else if(objectIn->getType() == ObjectLine){
                Line *line = (Line*)objectIn;
                SColor color = line->getColor();
                Sphere *from = (Sphere*)line->from;
                Sphere *to = (Sphere*)line->to;
                vector3df fromPos = from->getPosition();
                vector3df toPos = to->getPosition();
                float dist = fromPos.getDistanceFrom(toPos);
                float colorDiff = from->getColor().color - to->getColor().color;
                float xDiff = toPos.X - fromPos.X;
                float yDiff = toPos.Y - fromPos.Y;
                float zDiff = toPos.Z - fromPos.Z;

                edgeNameText->setText(line->getName());

                edgeRtext->setText(QString::number(color.getRed()));
                edgeGtext->setText(QString::number(color.getGreen()));
                edgeBtext->setText(QString::number(color.getBlue()));

                edgeDistText->setText(QString::number(dist));
                edgeRGBdiffText->setText(QString::number(colorDiff));
                edgeXdiffText->setText(QString::number(xDiff));
                edgeYdiffText->setText(QString::number(yDiff));
                edgeZdiffText->setText(QString::number(zDiff));

                edgeDistMaxText->setText(QString::number(line->distMax));
                edgeDistMinText->setText(QString::number(line->distMin));
                edgeDistWeightText->setText(QString::number(line->distWeight));

                edgeRGBdiffMaxText->setText(QString::number(line->rgbDiffMax));
                edgeRGBdiffMinText->setText(QString::number(line->rgbDiffMin));
                edgeRGBdiffWeightText->setText(QString::number(line->rgbDiffWeight));

                edgeXmaxText->setText(QString::number(line->xDiffMax));
                edgeXminText->setText(QString::number(line->xDiffMin));
                edgeXweightText->setText(QString::number(line->xDiffWeight));

                edgeYmaxText->setText(QString::number(line->yDiffMax));
                edgeYminText->setText(QString::number(line->yDiffMin));
                edgeYweightText->setText(QString::number(line->yDiffWeight));

                edgeZmaxText->setText(QString::number(line->zDiffMax));
                edgeZminText->setText(QString::number(line->zDiffMin));
                edgeZweightText->setText(QString::number(line->zDiffWeight));
            }
        }
    }

    void clearAllText(){
        xText->setText("");
        yText->setText("");
        zText->setText("");

        rText->setText("");
        gText->setText("");
        bText->setText("");
        nodeRGBWeightText->setText("");

        nameText->setText("");
        radiusText->setText("");

        //<edge panel>---------------------------------
        edgeNameText->setText("");

        edgeRtext->setText("");
        edgeGtext->setText("");
        edgeBtext->setText("");

        edgeDistText->setText("");
        edgeDistMaxText->setText("");
        edgeDistMinText->setText("");
        edgeDistWeightText->setText("");

        edgeRGBdiffText->setText("");
        edgeRGBdiffMaxText->setText("");
        edgeRGBdiffMinText->setText("");
        edgeRGBdiffWeightText->setText("");

        edgeXdiffText->setText("");
        edgeXmaxText->setText("");
        edgeXminText->setText("");
        edgeXweightText->setText("");

        edgeYdiffText->setText("");
        edgeYmaxText->setText("");
        edgeYminText->setText("");
        edgeYweightText->setText("");

        edgeZdiffText->setText("");
        edgeZmaxText->setText("");
        edgeZminText->setText("");
        edgeZweightText->setText("");
        //</edge panel>--------------------------------
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

        if(selectedObj != 0){
            if(selectedObj->getType() == ObjectSphere){
                emit setEdgePanelVisible(false);
                emit setNodePanelVisible(true);
            }else if(selectedObj->getType() == ObjectLine){
                emit setNodePanelVisible(false);
                emit setEdgePanelVisible(true);
            }

            toTextAllParameters(selectedObj);
        }
    }

    void unselect(){
        if(selectedObj != 0){
            selectedObj = 0;
            clearAllText();
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

    bool isSphere(Object *objectIn){
        if(objectIn != 0){
            if(objectIn->getType() == ObjectSphere){
                return true;
            }
        }
        return false;
    }

    bool isLIne(Object *objectIn){
        if(objectIn != 0){
            if(objectIn->getType() == ObjectLine){
                return true;
            }
        }
        return false;
    }

signals:
    void setNodePanelVisible(bool visible);
    void setEdgePanelVisible(bool visible);

public slots:
    void loop(){
        if(m_device->run()){
            video::IVideoDriver *driver = m_device->getVideoDriver();
            scene::ISceneManager *smgr = m_device->getSceneManager();
            scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
            vector3df camPos = smgr->getActiveCamera()->getPosition();
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

            if(gizmo->grabed && selectedObj != 0){
                toTextAllParameters(selectedObj);
            }

            if(toolStatus == ToolSphere){
                moveSphereAroundCamera(sphere);
            }

            gizmo->setObject(0);
            if(toolStatus == ToolMove && !isLIne(selectedObj)){
                gizmo->setObject((TransformableObject*)selectedObj);
            }

            if(toolStatus == ToolLineTo){
                line3d<f32> ray = collMan->getRayFromScreenCoordinates(mousePos, smgr->getActiveCamera());
                vector3df pos = camPos + ray.getVector().normalize()*5;

                SMaterial material;
                material.EmissiveColor = SColor(255, 255, 255, 255);
                driver->setMaterial(material);
                driver->draw3DLine(lineFrom->getPosition(), pos);
            }

            if(toolStatus == ToolMoveFromCamera && !isLIne(selectedObj)){
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

            //pc->render();
            core->renderAll();
            smgr->drawAll();

            driver->clearZBuffer();
            gizmo->render();

            driver->endScene();
        }
    }

    void createSphere(){
        sphereRadius = 5;
        if(checkFloat(radiusText->text())){
            sphereRadius = radiusText->text().toFloat();
        }

        toolStatus = ToolSphere;

        QString name = "sphere"+QString::number(sphereNum);

        sphere = core->addSphere(sphereRadius);
        sphere->setColor(SColor(255, rText->text().toInt(), gText->text().toInt(), bText->text().toInt()));
        sphere->setName(name);

        sphereNum++;
    }

    void createEdge(){
        toolStatus = ToolLineFrom;
        selectedObj = 0;
    }

    void createPointCloud(){
        if(checkInteger(pointCloudNum->text()) && checkFloat(pointCloudRadius->text())){
            float radius = pointCloudRadius->text().toFloat();
            int num = pointCloudNum->text().toInt();

            float size = 1.;
            if(checkFloat(pointCloudPointSize->text())){
                size = pointCloudPointSize->text().toFloat();
            }

            pointCloud = core->addPointCloud();
            pointCloud->setPointSize(size);

//            if(pointCloudRandomRGB->isChecked()){
//                for(int i=0; i<num; i++){
//                    pointCloud->addPoint(radius);
//                }
//            }

            for(int i=0; i<num; i++){
                pointCloud->addPoint(radius);
            }
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

    void applyX(){
        QString text = xText->text();
        if(checkFloat(text) && selectedObj != 0){
            if(selectedObj->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)selectedObj;
                vector3df pos = sph->getPosition();
                pos.X = text.toFloat();
                sph->setPosition(pos);
            }
        }
    }

    void applyY(){
        QString text = yText->text();
        if(checkFloat(text) && selectedObj != 0){
            if(selectedObj->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)selectedObj;
                vector3df pos = sph->getPosition();
                pos.Y = text.toFloat();
                sph->setPosition(pos);
            }
        }
    }

    void applyZ(){
        QString text = zText->text();
        if(checkFloat(text) && selectedObj != 0){
            if(selectedObj->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)selectedObj;
                vector3df pos = sph->getPosition();
                pos.Z = text.toFloat();
                sph->setPosition(pos);
            }
        }
    }

    void applyR(){
        QString text = rText->text();
        if(checkInteger(text) && selectedObj != 0){
            selectedObj->color.setRed(text.toInt());
        }
    }

    void applyG(){
        QString text = gText->text();
        if(checkInteger(text) && selectedObj != 0){
            selectedObj->color.setGreen(text.toInt());
        }
    }

    void applyB(){
        QString text = bText->text();
        if(checkInteger(text) && selectedObj != 0){
            selectedObj->color.setBlue(text.toInt());
        }
    }

    void applyName(){
        if(selectedObj != 0){
            selectedObj->setName(nameText->text().toStdString().c_str());
        }
    }

    void applyRadius(){
        QString text = radiusText->text();
        if(checkFloat(text) && selectedObj != 0){
            if(selectedObj->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)selectedObj;
                float r = radiusText->text().toFloat();
                sph->setScale(r);
            }
        }
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
            leftMouseButtonDown = true;
            leftButtonInitmousePos = m_device->getCursorControl()->getPosition();
            if(isSphere(selectedObj)){
                Sphere *sph = (Sphere*)selectedObj;
                initPos = sph->getPosition();
            }
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

        gizmo->mouseLeftButtonPressEvent();
    }

    void mouseReleaseEvent(QMouseEvent* event){
        irr::SEvent irrEvent;

        irrEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

        switch(event->button()){
        case Qt::LeftButton:
            irrEvent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
            leftMouseButtonDown = false;

            if((!gizmo->grabed || toolStatus == ToolNothing) && !(toolStatus == ToolMoveFromCamera)){
                if(hitedObj == 0){
                    unselect();
                }

                selectObject(hitedObj);
            }

            if(toolStatus == ToolSphere){
                toolStatus = ToolNothing;
                selectObject(sphere);
            }else if(toolStatus == ToolLineFrom && hitedObj != 0){
                if(hitedObj->getType() == ObjectSphere){
                    toolStatus = ToolLineTo;
                    lineFrom = (Sphere*)hitedObj;
                }
            }else if(toolStatus == ToolLineTo){
                if(hitedObj != 0){
                    if(hitedObj->getType() == ObjectSphere){
                        QString name = "edge"+QString::number(lineNum);
                        lineNum++;

                        selectedObj = core->addLine(lineFrom, (Sphere*)hitedObj);
                        selectedObj->setColor(SColor(255, rText->text().toInt(), gText->text().toInt(), bText->text().toInt()));
                        selectedObj->setName(name);

                        selectObject(selectedObj);
                        toolStatus = ToolNothing;
                    }
                }
            }

            gizmo->mouseLeftButtonReleaseEvent();

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
            gizmo->mouseMoveEvent(vector2d<s32>(event->x(), event->y()));
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

    Gizmo *gizmo;

    s32 shaderType;

    //toolStatus определяет какой инструмент выбран
    ToolStatus toolStatus;
    bool leftMouseButtonDown;
    bool rightMouseButtonDown;
    vector2d<s32> leftButtonInitmousePos;
    vector3df initPos;

    Sphere *sphere;
    float sphereRadius;
    int sphereNum;

    PointCloud *pointCloud;

    Line *line;
    Sphere *lineFrom;
    int lineNum;

    Object *selectedObj;
    Object *hitedObj;

    irr::IrrlichtDevice *m_device;
    ISceneManager *smgr;
    QTimer *m_timer;

    PointCloud *pc;
};

#endif // WIDGET_H

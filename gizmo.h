#include <irrlicht.h>
#include <qdebug.h>
#include <irrlichtwrap.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

class Gizmo{
public:
    Arrow* arrowX;
    Arrow* arrowY;
    Arrow* arrowZ;
    Plane* planeXZ;
    Plane* planeXY;
    Plane* planeZY;

    TransformableObject *hited;

    float arrowScale;
    float planesScale;

    ISceneManager *smgr;
    Core *core;
    vector2d<s32> mousePos;

    bool grabed;
    vector3df initPos;

    TransformableObject *object;

    Gizmo(ISceneManager *smgrIn, float gizmoScaleIn=1.){
        smgr = smgrIn;
        core = new Core(smgrIn);
        grabed = false;
        object = 0;

        arrowScale = 20*gizmoScaleIn;
        float arrowHeight = arrowScale;
        float cylHeight = arrowScale*0.75;
        float cylWidth = arrowScale*0.03;
        float coneWidth = arrowScale*0.05;

        arrowX = core->addArrow(arrowHeight, cylHeight, cylWidth, coneWidth);
        arrowX->setColor(SColor(255, 255, 0, 0));
        arrowX->setName("arrowX");
        arrowX->setRotation(vector3df(0, 0, -90));

        arrowY = core->addArrow(arrowHeight, cylHeight, cylWidth, coneWidth);
        arrowY->setColor(SColor(255, 0, 255, 0));
        arrowY->setName("arrowY");

        arrowZ = core->addArrow(arrowHeight, cylHeight, cylWidth, coneWidth);
        arrowZ->setColor(SColor(255, 0, 0, 255));
        arrowZ->setName("arrowZ");
        arrowZ->setRotation(vector3df(90, 0, 90));

        planesScale = 8*gizmoScaleIn;
        SColor planeColor = SColor(170, 200, 200, 0);

        planeXZ = core->addPlane(planesScale, planesScale);
        planeXZ->setPosition(vector3df(planesScale/2, 0, planesScale/2));
        planeXZ->setColor(planeColor);
        planeXZ->setName("planeXZ");

        planeXY = core->addPlane(planesScale, planesScale);
        planeXY->setPosition(vector3df(planesScale/2, planesScale/2, 0));
        planeXY->setColor(planeColor);
        planeXY->setName("planeXY");
        planeXY->setRotation(vector3df(90, 0, 0));

        planeZY = core->addPlane(planesScale, planesScale);
        planeZY->setPosition(vector3df(0, planesScale/2, planesScale/2));
        planeZY->setColor(planeColor);
        planeZY->setName("planeZY");
        planeZY->setRotation(vector3df(0, 0, -90));
    }

    void update(){
        if(object != 0){
            vector3df objPos = object->getPosition();
            ICameraSceneNode *cam = smgr->getActiveCamera();
            vector3df cameraPosition = cam->getPosition();

            float step = planesScale/2;
            vector3df planeZYpos = vector3df(0, step, step);
            vector3df planeXYpos = vector3df(step, step, 0);
            vector3df planeXZpos = vector3df(step, 0, step);

            if(cameraPosition.Z <= objPos.Z ){
                planeXY->setRotation(vector3df(-90, 0, 0));
                arrowZ->setRotation(vector3df(-90, 0, 0));
                planeZYpos.Z = -step;
                planeXZpos.Z = -step;
            }else{
                planeXY->setRotation(vector3df(90, 0, 0));
                arrowZ->setRotation(vector3df(90, 0, 0));
                planeZYpos.Z = step;
                planeXZpos.Z = step;
            }

            if(cameraPosition.X <= objPos.X ){
                planeZY->setRotation(vector3df(0, 0, 90));
                arrowX->setRotation(vector3df(0, 0, 90));
                planeXYpos.X = -step;
                planeXZpos.X = -step;

            }else{
                planeZY->setRotation(vector3df(0, 0, -90));
                arrowX->setRotation(vector3df(0, 0, -90));
                planeXYpos.X = step;
                planeXZpos.X = step;
            }

            if(cameraPosition.Y <= objPos.Y ){
                planeXZ->setRotation(vector3df(0, 0, 180));
                arrowY->setRotation(vector3df(0, 0, 180));

                planeZYpos.Y = -step;
                planeXYpos.Y = -step;
            }else{
                planeXZ->setRotation(vector3df(0, 0, 0));
                arrowY->setRotation(vector3df(0, 0, 0));

                planeZYpos.Y = step;
                planeXYpos.Y = step;
            }

            vector3df pos = cameraPosition + (objPos-cameraPosition).normalize()*70;
            planeXZ->setPosition(vector3df(planeXZpos.X+pos.X, planeXZpos.Y+pos.Y, planeXZpos.Z+pos.Z));
            planeXY->setPosition(vector3df(planeXYpos.X+pos.X, planeXYpos.Y+pos.Y, planeXYpos.Z+pos.Z));
            planeZY->setPosition(vector3df(planeZYpos.X+pos.X, planeZYpos.Y+pos.Y, planeZYpos.Z+pos.Z));

            arrowX->setPosition(pos);
            arrowY->setPosition(pos);
            arrowZ->setPosition(pos);
        }
    }

    void render(){
        glEnable(GL_LIGHTING); //если свет отлючить то вся гизма при пертаскивании становится белой

        update();

        if(object != 0){
            core->unselectAll();

            if(grabed == false){
                hited = (TransformableObject*)core->getObjectUnderCursor(mousePos);
            }
            if(hited != 0){
                hited->setSelected(true);
            }

            core->renderAll();
        }
    }

    void resetColor(){
        hited = 0;
        arrowX->setColor(SColor(255, 255, 0, 0));
        arrowY->setColor(SColor(255, 0, 255, 0));
        arrowZ->setColor(SColor(255, 0, 0, 255));
    }


    void mouseLeftButtonPressEvent(){
        if(!grabed && object!=0){
            initPos = object->getPosition();
        }
        if(hited != 0){
            grabed = true;
        }
    }

    void mouseLeftButtonReleaseEvent(){
        grabed = false;
    }

    void mouseMoveEvent(vector2d<s32> mousePosIn){
        mousePos = mousePosIn;

        if(grabed && hited != 0 && object != 0){
            ICameraSceneNode *cam = smgr->getActiveCamera();

            line3d<f32> ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(mousePosIn, cam);

            vector3df newPos = initPos;
            if(hited == arrowX){
                float k = (ray.end.X - ray.start.X)/(ray.end.Z - ray.start.Z);
                float c = ray.start.X - k*ray.start.Z;

                newPos +=  vector3df(c, 0, 0);
            }else if(hited == arrowY){
                float k = (ray.end.Y - ray.start.Y)/(ray.end.Z - ray.start.Z);
                float c = ray.start.Y - k*ray.start.Z;

                newPos += vector3df(0, c, 0);
            }else if(hited == arrowZ){
                float k = (ray.end.Z - ray.start.Z)/(ray.end.X - ray.start.X);
                float c = ray.start.Z - k*ray.start.X;

                newPos += vector3df(0, 0, c);
            }

            object->setPosition(newPos);
        }
    }


    void setObject(TransformableObject *objectIn){
        object = objectIn;
    }

    void unsetObject(){
        object = 0;
    }
};


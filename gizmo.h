#include <irrlicht.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

class Gizmo{
public:
    scene::ISceneNode* arrowX;
    scene::ISceneNode* arrowY;
    scene::ISceneNode* arrowZ;
    scene::ISceneNode* planeXZ;
    scene::ISceneNode* planeXY;
    scene::ISceneNode* planeZY;

    float arrowScale;
    float planesScale;

    Gizmo(scene::ISceneManager *smgrIn, float gizmoScaleIn=1.){
        arrowScale = 20*gizmoScaleIn;
        SColor arrowC = SColor(255, 0, 255, 0);
        arrowY = smgrIn->addMeshSceneNode(smgrIn->addArrowMesh("arrowY", arrowC, arrowC, 4, 8, arrowScale, arrowScale*0.75, arrowScale*0.03, arrowScale*0.05)->getMesh(0));
        arrowY->getMaterial(0).EmissiveColor = arrowY->getMaterial(1).EmissiveColor = arrowC;

        arrowC = SColor(255, 255, 0, 0);
        arrowX = smgrIn->addMeshSceneNode(smgrIn->addArrowMesh("arrowX", arrowC, arrowC, 4, 8, arrowScale, arrowScale*0.75, arrowScale*0.03, arrowScale*0.05)->getMesh(0));
        arrowX->getMaterial(0).EmissiveColor = arrowX->getMaterial(1).EmissiveColor = arrowC;
        arrowX->setRotation(vector3df(0, 0, -90));

        arrowC = SColor(255, 0, 0, 255);
        arrowZ = smgrIn->addMeshSceneNode(smgrIn->addArrowMesh("arrowZ", arrowC, arrowC, 4, 8, arrowScale, arrowScale*0.75, arrowScale*0.03, arrowScale*0.05)->getMesh(0));
        arrowZ->getMaterial(0).EmissiveColor = arrowZ->getMaterial(1).EmissiveColor = arrowC;
        arrowZ->setRotation(vector3df(90, 0, 0));

        planesScale = 8*gizmoScaleIn;
        SMaterial material = SMaterial();
        material.EmissiveColor = SColor(170, 200, 200, 0);
        planeXZ = smgrIn->addMeshSceneNode(smgrIn->addHillPlaneMesh("planeXZ", dimension2d<f32>(planesScale, planesScale), dimension2d<u32>(1, 1), &material));
        planeXZ->setPosition(vector3df(planesScale/2, 0, planesScale/2));

        planeXY = smgrIn->addMeshSceneNode(smgrIn->addHillPlaneMesh("planeXY", dimension2d<f32>(planesScale, planesScale), dimension2d<u32>(1, 1), &material));
        planeXY->setRotation(vector3df(90, 0, 0));
        planeXY->setPosition(vector3df(planesScale/2, planesScale/2, 0));

        planeZY = smgrIn->addMeshSceneNode(smgrIn->addHillPlaneMesh("planeZY", dimension2d<f32>(planesScale, planesScale), dimension2d<u32>(1, 1), &material));
        planeZY->setRotation(vector3df(0, 0, -90));
        planeZY->setPosition(vector3df(0, planesScale/2, planesScale/2));
    }

    void update(vector3df &cameraPositionIn, vector3df objPosIn){
        float step = planesScale/2;
        vector3df planeZYpos = vector3df(0, step, step);
        vector3df planeXYpos = vector3df(step, step, 0);
        vector3df planeXZpos = vector3df(step, 0, step);

        if(cameraPositionIn.Z <= objPosIn.Z ){
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

        if(cameraPositionIn.X <= objPosIn.Y ){
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

        if(cameraPositionIn.Y <= objPosIn.Y ){
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

        vector3df pos = cameraPositionIn + (objPosIn-cameraPositionIn).normalize()*70;
        planeXZ->setPosition(vector3df(planeXZpos.X+pos.X, planeXZpos.Y+pos.Y, planeXZpos.Z+pos.Z));
        planeXY->setPosition(vector3df(planeXYpos.X+pos.X, planeXYpos.Y+pos.Y, planeXYpos.Z+pos.Z));
        planeZY->setPosition(vector3df(planeZYpos.X+pos.X, planeZYpos.Y+pos.Y, planeZYpos.Z+pos.Z));
        arrowX->setPosition(pos);
        arrowY->setPosition(pos);
        arrowZ->setPosition(pos);
    }

    void render(){
        arrowX->render();
        arrowY->render();
        arrowZ->render();
        planeXZ->render();
        planeXY->render();
        planeZY->render();
    }
};

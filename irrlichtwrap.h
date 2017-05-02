#include <iostream>
#include <irrlicht.h>
#include <qdebug.h>
#include <qfile.h>
#include <QUuid>
#include <qjsonobject.h>
#include <QJsonDocument>
#include <GL/gl.h>

using namespace std;

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace gui;

enum ObjectType{
    ObjectSphere,
    ObjectLine,
    ObjectCylinder,
    ObjectArrow,
    ObjectPlane,
    ObjectPointCloud
};

class Object{
protected:
    ISceneManager* smgr;
    SMaterial material;
    QString name;
    ObjectType objectType;
    QString id;

    bool visible;
    bool selected;

public:
    SColor color;

    Object(ISceneManager* smgrIn, ObjectType objectTypeIn){
        objectType = objectTypeIn;
        smgr = smgrIn;
        visible = true;
        id = QUuid().toString();
    }

    virtual void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){

    }

    void setColor(SColor colorIn){
        color = colorIn;
    }

    SColor getColor(){
        return color;
    }

    void setName(QString nameIn){
        name = nameIn;
    }

    QString getName(){
        return name;
    }

    ObjectType getType(){
        return objectType;
    }

    void setVisible(bool visibleIn){
        visible = visibleIn;
    }

    void setSelected(bool selectedIn){
        selected = selectedIn;
    }

    virtual QJsonObject toJSON(){

    }

    QString getId(){
        return id;
    }
};

class TransformableObject: public Object{
protected:
    matrix4 transform;

public:
    TransformableObject(ISceneManager* smgrIn, ObjectType objectTypeIn): Object(smgrIn, objectTypeIn){

    }

    vector3df getPosition(){
        return transform.getTranslation();
    }

    vector3df getRotation(){
        return transform.getRotationDegrees();
    }

    float getScale(){
        return transform.getScale().X;
    }

    void setPosition(vector3df positionIn){
        transform.setTranslation(positionIn);
    }

    void setRotation(vector3df rotationIn){
        transform.setRotationDegrees(rotationIn);
    }

    void setScale(float scaleIn){
        transform.setScale(scaleIn);
    }
};

class Arrow: public TransformableObject{
private:
    IMesh *mesh;

public:
    Arrow(ISceneManager* smgrIn, float arrowHeightIn, float cylHeightIn, float cylWidthIn, float coneWidthIn): TransformableObject(smgrIn, ObjectArrow){
        mesh = smgr->getGeometryCreator()->createArrowMesh(4, 8, arrowHeightIn, cylHeightIn, cylWidthIn, coneWidthIn);
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){
        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

            if (mesh && driver)
            {
                if(useColorId){
                    material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());
                }else{
                    if(selected){
                        material.EmissiveColor.set(255, 255, 255, 0);
                    }else{
                        material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                    }
                }

                driver->setMaterial(material);
                driver->setTransform(video::ETS_WORLD, transform);
                driver->drawMeshBuffer(mesh->getMeshBuffer(0));
                driver->drawMeshBuffer(mesh->getMeshBuffer(1));
            }
        }
    }

    QJsonObject toJSON(){

    }
};

class Cylinder: public TransformableObject{
private:
    IMesh *mesh;

public:
    Cylinder(ISceneManager* smgrIn, float radiusIn, float heightIn): TransformableObject(smgrIn, ObjectCylinder){
        mesh = smgr->getGeometryCreator()->createCylinderMesh(radiusIn, heightIn, 12);
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){
        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

            if (mesh && driver)
            {
                if(useColorId){
                    material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());
                }else{
                    if(selected){
                        material.EmissiveColor.set(255, 255-color.getRed(), 255-color.getGreen(), 255-color.getBlue());
                    }else{
                        material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                    }
                }

                driver->setMaterial(material);
                driver->setTransform(video::ETS_WORLD, transform);
                driver->drawMeshBuffer(mesh->getMeshBuffer(0));
            }
        }
    }

    QJsonObject toJSON(){

    }
};

class Plane: public TransformableObject{
private:
    IMesh *mesh;

public:
    Plane(ISceneManager* smgrIn, float widthIn, float heightIn): TransformableObject(smgrIn, ObjectPlane){
        mesh = smgr->getGeometryCreator()->createPlaneMesh(dimension2d<f32>(widthIn, heightIn), dimension2d<u32>(1, 1));
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){
        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

            if (mesh && driver)
            {
                if(useColorId){
                    material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());
                }else{
                    if(selected){
                        material.EmissiveColor.set(255, 255, 255, 0);
                    }else{
                        material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                    }
                }

                driver->setMaterial(material);
                driver->setTransform(video::ETS_WORLD, transform);
                driver->drawMeshBuffer(mesh->getMeshBuffer(0));
            }
        }
    }

    QJsonObject toJSON(){

    }
};

class Sphere: public TransformableObject{
private:
    IMesh *mesh;

public:
    float rgbWeight;

    Sphere(ISceneManager* smgrIn, float radiusIn): TransformableObject(smgrIn, ObjectSphere){
        mesh = smgr->getGeometryCreator()->createSphereMesh(1., 12, 12);
        setScale(radiusIn);

        rgbWeight = 0;
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){
        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

            if (mesh && driver)
            {
                if(useColorId){
                    material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());
                }else{
                    if(selected){
                        float scale = this->getScale();
                        this->setScale(scale*1.01);

                        material.EmissiveColor.set(255, 255-color.getRed(), 255-color.getGreen(), 255-color.getBlue());
                        material.Wireframe = true;
                        driver->setMaterial(material);
                        driver->setTransform(video::ETS_WORLD, transform);
                        driver->drawMeshBuffer(mesh->getMeshBuffer(0));

                        this->setScale(scale);
                    }

                    material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                }

                material.Wireframe = false;
                driver->setMaterial(material);
                driver->setTransform(video::ETS_WORLD, transform);
                driver->drawMeshBuffer(mesh->getMeshBuffer(0));
            }
        }
    }

    QJsonObject toJSON(){
        vector3df pos = getPosition();
        SColor color = getColor();

        QJsonObject json;
        json["name"] = name;
        json["id"] = id;

        json["x"] = pos.X;
        json["y"] = pos.Y;
        json["z"] = pos.Z;

        json["r"] = QString::number(color.getRed());
        json["g"] = QString::number(color.getGreen());
        json["b"] = QString::number(color.getBlue());
    }
};

class Line: public Object{
private:
    float width;

public:
    Sphere *from;
    Sphere *to;

    float xDiffMin;
    float yDiffMin;
    float zDiffMin;

    float xDiffMax;
    float yDiffMax;
    float zDiffMax;

    float xDiffWeight;
    float yDiffWeight;
    float zDiffWeight;

    float distMin;
    float distMax;
    float distWeight;

    float rgbDiffMin;
    float rgbDiffMax;
    float rgbDiffWeight;

    Line(ISceneManager* smgrIn, Sphere *fromIn, Sphere *toIn): Object(smgrIn, ObjectLine){
        from = fromIn;
        to = toIn;
        material.EmissiveColor = SColor(255, 255, 255, 255);
        width = 1.;
        selected = false;

        xDiffMin = 0;
        yDiffMin = 0;
        zDiffMin = 0;

        xDiffMax = 0;
        yDiffMax = 0;
        zDiffMax = 0;

        xDiffWeight = 0;
        yDiffWeight = 0;
        zDiffWeight = 0;

        distMin = 0;
        distMax = 0;
        distWeight = 0;

        rgbDiffMin = 0;
        rgbDiffMax = 0;
        rgbDiffWeight = 0;
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0))
    {
        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

            float defaultWidth = 1.;
            glGetFloatv(GL_LINE_WIDTH, &defaultWidth);

            if(useColorId){
                material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());
                glLineWidth(10.);
            }else{
                if(selected){
                    material.EmissiveColor.set(255, 255-color.getRed(), 255-color.getGreen(), 255-color.getBlue());
                }else{
                    material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                }
                glLineWidth(width);
            }

            driver->setMaterial(material);
            driver->setTransform(ETS_WORLD, core::IdentityMatrix);
            driver->draw3DLine(from->getPosition(), to->getPosition());

            glLineWidth(defaultWidth);
        }

    }

    void setWidth(float widthIn){
        width = widthIn;
    }

    QJsonObject toJSON(){
        SColor color = getColor();

        QJsonObject json;
        json["name"] = name;
        json["id"] = id;

        json["r"] = QString::number(color.getRed());
        json["g"] = QString::number(color.getGreen());
        json["b"] = QString::number(color.getBlue());

        json["fromId"] = from->getId();
        json["toId"] = to->getId();

        json["xDiffMin"] = xDiffMin;
        json["yDiffMin"] = yDiffMin;
        json["zDiffMin"] = zDiffMin;

        json["xDiffMax"] = xDiffMax;
        json["yDiffMax"] = yDiffMax;
        json["zDiffMax"] = zDiffMax;

        json["xDiffWeight"] = xDiffWeight;
        json["yDiffWeight"] = yDiffWeight;
        json["zDiffWeight"] = zDiffWeight;

        json["distMin"] = distMin;
        json["distMax"] = distMax;
        json["distWeight"] = distWeight;

        json["rgbDiffMin"] = rgbDiffMin;
        json["rgbDiffMax"] = rgbDiffMax;
        json["rgbDiffWeight"] = rgbDiffWeight;
    }
};

class Core{
private:
    ISceneManager* smgr;
    vector<Object*> objects;
    ITexture* rt;
    float lineWidth;

    SColor getPixel(ITexture *textureIn, int xIn, int yIn){
        SColor color;
        dimension2d<u32> size = textureIn->getSize();
        int index = (xIn + size.Width*yIn)*4;

        //B8 G8 R8 A8
        u8 *buf = (u8*)textureIn->lock();
        color.setBlue(buf[index]);
        color.setGreen(buf[index+1]);
        color.setRed(buf[index+2]);
        color.setAlpha(buf[index+3]);
        textureIn->unlock();

        return color;
    }
    SColor getPixel(ITexture *textureIn, vector2d<s32> positionIn){
        return getPixel(textureIn, positionIn.X, positionIn.Y);
    }

    SColor getColorId(int lineIndexIn){
        u8 r = (lineIndexIn+1)%255;
        u8 g = (lineIndexIn/255)%255;
        u8 b = (lineIndexIn/(255*255))%255;

        return SColor(255, r, g, b);
    }

    Object *getObjectByColorId(SColor colorIdIn){
        u32 r = colorIdIn.getRed();
        u32 g = colorIdIn.getGreen()*255;
        u32 b = colorIdIn.getBlue()*255*255;
        u32 index = r+g+b;

        if(index == 0){
            return 0;
        }

        index--;

        if(index < objects.size()){
            return objects[index];
        }

        return 0;
    }

public:
    Core(ISceneManager* smgrIn){
        smgr = smgrIn;
        lineWidth = 1.;
        IVideoDriver *driver = smgrIn->getVideoDriver();
        rt = driver->addRenderTargetTexture(driver->getScreenSize());
    }

    Sphere *addSphere(float radiusIn){
        Sphere *sph = new Sphere(smgr, radiusIn);
        objects.push_back(sph);
        return sph;
    }

    Line *addLine(Sphere *fromIn, Sphere *toIn){
        Line *line = new Line(smgr, fromIn, toIn);
        line->setWidth(lineWidth);
        objects.push_back(line);
        return line;
    }

    Arrow *addArrow(float arrowHeightIn, float cylHeightIn, float cylWidthIn, float coneWidthIn){
        Arrow *arrow = new Arrow(smgr, arrowHeightIn, cylHeightIn, cylWidthIn, coneWidthIn);
        objects.push_back(arrow);
        return arrow;
    }

    Cylinder *addCylinder(float radiusIn, float heightIn){
        Cylinder *cyl = new Cylinder(smgr, radiusIn, heightIn);
        objects.push_back(cyl);
        return cyl;
    }

    Plane *addPlane(float widthIn, float heightIn){
        Plane *plane = new Plane(smgr, widthIn, heightIn);
        objects.push_back(plane);
        return plane;
    }

    void renderAll(bool useColorId=false){
        if(useColorId){
            for(int i=0; i<objects.size(); i++){
                objects[i]->render(useColorId, getColorId(i));
            }
        }else{
            for(int i=0; i<objects.size(); i++){
                objects[i]->render();
            }
        }
    }

    void unselectAll(){
        for(int i=0; i<objects.size(); i++){
            objects[i]->setSelected(false);
        }
    }

    void setAllLinesWidth(float widthIn){
        lineWidth = widthIn;

        for(int i=0; i<objects.size(); i++){
            if(objects[i]->getType() == ObjectLine){
                Line *line = (Line*)objects[i];
                line->setWidth(lineWidth);
            }
        }
    }

    ISceneManager *getSceneManager(){
        return smgr;
    }

    //возвращает объект под курсором или 0
    Object *getObjectUnderCursor(vector2d<s32> mousePosIn){
        IVideoDriver *driver = smgr->getVideoDriver();

        driver->setRenderTarget(rt, true, true, SColor(255, 0, 0, 0));
        renderAll(true);
        driver->setRenderTarget(0, false, false, 0);

        return getObjectByColorId(getPixel(rt, mousePosIn));
    }

    Sphere *sphereFromJSON(QJsonObject *jsonIn){

    }

    void toDB(QString dbFileNameIn){
        QFile saveFile(dbFileNameIn);

        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open DB file.");
            return;
        }

        QJsonObject jsonRoot;
        QJsonDocument doc(jsonRoot);
        saveFile.write(doc.toJson());

        return;
    }

    void fromDB(QString dbFileNameIn){
        QFile loadFile(dbFileNameIn);

        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

        QByteArray data = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(data));

        return;
    }
};

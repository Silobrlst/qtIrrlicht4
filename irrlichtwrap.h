#include <iostream>
#include <vector>

#include <qdebug.h>
#include <qfile.h>
#include <QUuid>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <QJsonDocument>

#include <GL/gl.h>

#include <irrlicht.h>
#include "tinyply.h"
#include "nanoflann.hpp"

using namespace std;
using namespace nanoflann;
using namespace tinyply;

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
    ObjectType objectType;

    SMaterial material;
    QString name;
    QString id;

    bool visible;
    bool selected;

    QJsonObject toJSONobject(){
        SColor color = getColor();

        QJsonObject rgbaJSON;
        rgbaJSON["r"] = (int)color.getRed();
        rgbaJSON["g"] = (int)color.getGreen();
        rgbaJSON["b"] = (int)color.getBlue();
        rgbaJSON["a"] = (int)color.getAlpha();

        QJsonObject json;
        json["name"] = name;
        json["id"] = id;
        json["rgb"] = rgbaJSON;

        return json;
    }

    void fromJSONobject(QJsonObject jsonIn){
        id = jsonIn["id"].toString();

        setName(jsonIn["name"].toString());

        QJsonObject rgbaJSON = jsonIn["rgba"].toObject();

        SColor c;
        c.setRed(rgbaJSON["r"].toInt());
        c.setGreen(rgbaJSON["g"].toInt());
        c.setBlue(rgbaJSON["b"].toInt());
        c.setAlpha(rgbaJSON["a"].toInt());
        setColor(c);
    }

public:
    SColor color;

    Object(ISceneManager* smgrIn, ObjectType objectTypeIn){
        objectType = objectTypeIn;
        smgr = smgrIn;
        visible = true;
        id = QUuid::createUuid().toString();
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

    virtual void fromJSON(QJsonObject jsonIn){

    }

    QString getId(){
        return id;
    }
};

class TransformableObject: public Object{
protected:
    matrix4 transform;

    QJsonObject toJSONtransformableObject(){
        QJsonObject json = toJSONobject();
        vector3df pos = getPosition();

        QJsonObject xyzJSON;
        xyzJSON["x"] = (float)pos.X;
        xyzJSON["y"] = (float)pos.Y;
        xyzJSON["z"] = (float)pos.Z;

        json["scale"] = getScale();
        json["xyz"] = xyzJSON;

        return json;
    }

    void fromJSONtransformableObject(QJsonObject jsonIn){
        fromJSONobject(jsonIn);

        QJsonObject xyzJSON = jsonIn["xyz"].toObject();

        vector3df pos;
        pos.X = xyzJSON["x"].toDouble();
        pos.Y = xyzJSON["y"].toDouble();
        pos.Z = xyzJSON["z"].toDouble();

        setPosition(pos);
        setScale(jsonIn["scale"].toDouble());
    }

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

    matrix4 *getMatrix(){
        return &transform;
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
        glEnable(GL_LIGHTING);

        if(visible){
            IVideoDriver* driver = smgr->getVideoDriver();

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

            material.Wireframe = true;

            if(useColorId){
                material.Wireframe = false;
            }

            driver->setMaterial(material);
            driver->setTransform(video::ETS_WORLD, transform);
            driver->drawMeshBuffer(mesh->getMeshBuffer(0));
        }
    }

    QJsonObject toJSON(){
        QJsonObject json = toJSONtransformableObject();

        json["rgbWeight"] = rgbWeight;

        return json;
    }

    void fromJSON(QJsonObject jsonIn){
        fromJSONtransformableObject(jsonIn);

        rgbWeight = jsonIn["rgbWeight"].toDouble();
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
        glEnable(GL_LIGHTING);

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

        QJsonObject json = toJSONobject();

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

        return json;
    }

    void fromJSON(QJsonObject jsonIn){
        fromJSONobject(jsonIn);

        xDiffMin = jsonIn["xDiffMin"].toDouble();
        yDiffMin = jsonIn["yDiffMin"].toDouble();
        zDiffMin = jsonIn["zDiffMin"].toDouble();

        xDiffMax = jsonIn["xDiffMax"].toDouble();
        yDiffMax = jsonIn["yDiffMax"].toDouble();
        zDiffMax = jsonIn["zDiffMax"].toDouble();

        xDiffWeight = jsonIn["xDiffWeight"].toDouble();
        yDiffWeight = jsonIn["yDiffWeight"].toDouble();
        zDiffWeight = jsonIn["zDiffWeight"].toDouble();

        distMin = jsonIn["distMin"].toDouble();
        distMax = jsonIn["distMax"].toDouble();
        distWeight = jsonIn["distWeight"].toDouble();

        rgbDiffMin = jsonIn["rgbDiffMin"].toDouble();
        rgbDiffMax = jsonIn["rgbDiffMax"].toDouble();
        rgbDiffWeight = jsonIn["rgbDiffWeight"].toDouble();
    }
};

class PointCloud: public TransformableObject{
    std::vector<float> verts;
    std::vector<uint8_t> colors;

    uint32_t vertexCount;

    float pointSize;

    //vertex to JSON
    QJsonObject toJSONvertex(u32 indexIn){
        u32 vertexIndex = indexIn*3;
        u32 colorIndex = indexIn*4;
        float x = verts[vertexIndex];
        float y = verts[vertexIndex+1];
        float z = verts[vertexIndex+2];
        float r = colors[colorIndex];
        float g = colors[colorIndex+1];
        float b = colors[colorIndex+2];
        float a = colors[colorIndex+3];

        QJsonObject json;

        json["x"] = x;
        json["y"] = y;
        json["z"] = z;

        json["r"] = r;
        json["g"] = g;
        json["b"] = b;
        json["a"] = a;

        return json;
    }

    void fromJSONvertex(QJsonObject jsonIn){
        float x = jsonIn["x"].toDouble();
        float y = jsonIn["y"].toDouble();
        float z = jsonIn["z"].toDouble();
        int r = jsonIn["r"].toInt();
        int g = jsonIn["g"].toInt();
        int b = jsonIn["b"].toInt();

        addPoint(x, y, z, r, g, b);
    }

public:
    PointCloud(ISceneManager *smgrIn, QString fileNameIn): TransformableObject(smgrIn, ObjectPointCloud){
        pointSize = 1.;

        std::ifstream ss(fileNameIn.toStdString(), std::ios::binary);
        PlyFile file(ss);

        vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
        file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

        file.read(ss);
    }

    PointCloud(ISceneManager *smgrIn): TransformableObject(smgrIn, ObjectPointCloud){
        pointSize = 1.;
        vertexCount = 0;
    }

    void render(bool useColorId=false, SColor colorIdIn=SColor(255, 0, 0, 0)){
        if(visible){
            if(useColorId){
                material.EmissiveColor.set(255, colorIdIn.getRed(), colorIdIn.getGreen(), colorIdIn.getBlue());

                glPointSize(max(pointSize, 10.0f));
            }else{
                if(selected){
                    material.EmissiveColor.set(255, 255, 255, 255);
                }else{
                    material.EmissiveColor.set(255, color.getRed(), color.getGreen(), color.getBlue());
                }

                glPointSize(pointSize);
            }

            if(selected || useColorId){
                SColor c = material.EmissiveColor;
                glColor4ub(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
            }

            IVideoDriver *driver = smgr->getVideoDriver();
            driver->setTransform(ETS_WORLD, transform);

            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);

            glBegin(GL_POINTS);

            for(int i=0; i<vertexCount; i++){
                u32 vertexIndex = i*3;
                u32 colorIndex = i*4;
                float x = verts[vertexIndex];
                float y = verts[vertexIndex+1];
                float z = verts[vertexIndex+2];
                float r = colors[colorIndex];
                float g = colors[colorIndex+1];
                float b = colors[colorIndex+2];
                float a = colors[colorIndex+3];

                if(!selected && !useColorId){
                    glColor4ub(r, g, b, a);
                }

                glVertex3f(x, y, z);
            }
            glEnd();
        }
    }

    void setPointSize(float pointSizeIn){
        pointSize = pointSizeIn;
    }

    void addPoint(float xIn, float yIn, float zIn, u8 rIn, u8 gIn, u8 bIn){
        verts.push_back(xIn);
        verts.push_back(yIn);
        verts.push_back(zIn);

        colors.push_back(rIn);
        colors.push_back(gIn);
        colors.push_back(bIn);
        colors.push_back(255);

        vertexCount++;
    }

    void addPoint(float xIn, float yIn, float zIn){
        u8 r = rand()%255;
        u8 g = rand()%255;
        u8 b = rand()%255;

        addPoint(xIn, yIn, zIn, r, g, b);
    }

    void addPoint(float radiusIn){
        float r = 0;

        r = rand(); r/=(RAND_MAX);
        float x = radiusIn*(r - 0.5);

        r = rand(); r/=(RAND_MAX);
        float y = radiusIn*(r - 0.5);

        r = rand(); r/=(RAND_MAX);
        float z = radiusIn*(r - 0.5);

        addPoint(x, y, z);
    }

    QJsonObject toJSON(){
        QJsonObject json = toJSONtransformableObject();
        QJsonArray vertices;

        for(u32 i=0; i<vertexCount; i++){
            vertices.append(toJSONvertex(i));
        }

        json["vertices"] = vertices;

        json["pointSize"] = pointSize;

        return json;
    }

    void fromJSON(QJsonObject jsonIn){
        pointSize = jsonIn["pointSize"].toDouble();

        QJsonArray vertices = jsonIn["vertices"].toArray();
        for(int i = 0; i < vertices.size(); i++){
            fromJSONvertex(vertices[i].toObject());
        }
    }


    inline size_t kdtree_get_point_count() const { return vertexCount; }

    inline float kdtree_distance(const float *p1, const size_t idx_p2, size_t /*size*/) const
    {
        u32 vertexIndex = idx_p2*3;
        float x = verts[vertexIndex];
        float y = verts[vertexIndex+1];
        float z = verts[vertexIndex+2];

        const float d0=p1[0]-x;
        const float d1=p1[1]-y;
        const float d2=p1[2]-z;
        return sqrt(d0*d0+d1*d1+d2*d2);
    }

    inline float kdtree_get_pt(const size_t idx, int dim) const
    {
        u32 vertexIndex = idx*3;
        float x = verts[vertexIndex];
        float y = verts[vertexIndex+1];
        float z = verts[vertexIndex+2];

        if (dim==0) return x;
        else if (dim==1) return y;
        else return z;
    }

    template <class BBOX> bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }


    int checkWithSphere(vector3df positionIn, float radiusIn){
        // construct a kd-tree index:
        typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3> my_kd_tree_t;
        my_kd_tree_t index(3, *this, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
        index.buildIndex();

        const float queryPos[3] = {positionIn.X, positionIn.Y, positionIn.Z};

        const float search_radius = static_cast<float>(radiusIn);
        std::vector<std::pair<size_t,float>> ret_matches;

        nanoflann::SearchParams params;

        const size_t nMatches = index.radiusSearch(&queryPos[0],search_radius, ret_matches, params);

//        cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
//        for (size_t i=0;i<nMatches;i++)
//            cout << "idx["<< i << "]=" << ret_matches[i].first << " dist["<< i << "]=" << ret_matches[i].second << endl;
//        cout << "\n";

        return nMatches;
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


    void sphereFromJSON(QJsonObject jsonIn){
        Sphere *sph = addSphere(0);
        sph->fromJSON(jsonIn);
    }

    void lineFromJSON(QJsonObject jsonIn){
        Sphere *from;
        Sphere *to;

        for(int i=0; i<objects.size(); i++){
            if(objects[i]->getType() == ObjectSphere){
                Sphere *sph = (Sphere*)objects[i];
                if(sph->getId().compare(jsonIn["fromId"].toString()) == 0){
                    from = sph;
                }
                if(sph->getId().compare(jsonIn["toId"].toString()) == 0){
                    to = sph;
                }
            }
        }

        Line *line = addLine(from, to);
        line->fromJSON(jsonIn);
    }

    void pointCloudFromJSON(QJsonObject jsonIn){
        PointCloud *pc = addPointCloud();
        pc->fromJSON(jsonIn);

        qInfo()<<pc->checkWithSphere(vector3df(0, 0, 0), 10);
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

    PointCloud *addPointCloud(QString fileNameIn=0){
        PointCloud *pc;

        if(fileNameIn != 0){
            pc = new PointCloud(smgr, fileNameIn);
        }else{
            pc = new PointCloud(smgr);
        }

        objects.push_back(pc);

        return pc;
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


    void toJSONfile(QString jsonFileNameIn){
        QFile saveFile(jsonFileNameIn);

        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open DB file.");
            return;
        }

        QJsonObject json;

        QJsonArray spheres;
        QJsonArray lines;
        QJsonArray pointClouds;
        for(int i=0; i<objects.size(); i++){
            if(objects[i]->getType() == ObjectSphere){
                spheres.append(objects[i]->toJSON());
            }
            if(objects[i]->getType() == ObjectLine){
                lines.append(objects[i]->toJSON());
            }
            if(objects[i]->getType() == ObjectPointCloud){
                pointClouds.append(objects[i]->toJSON());
            }
        }

        json["spheres"] = spheres;
        json["lines"] = lines;
        json["pointClouds"] = pointClouds;

        QJsonDocument doc(json);
        saveFile.write(doc.toJson());

        return;
    }

    void fromJSONfile(QString jsonFileNameIn){
        QFile loadFile(jsonFileNameIn);

        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

        QByteArray data = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(data));

        QJsonObject json = loadDoc.object();

        //first add spheres
        QJsonArray spheres = json["spheres"].toArray();
        for(int i = 0; i < spheres.size(); i++){
            sphereFromJSON(spheres[i].toObject());
        }

        QJsonArray lines = json["lines"].toArray();
        for(int i = 0; i < lines.size(); i++){
            lineFromJSON(lines[i].toObject());
        }

        QJsonArray pointClouds = json["pointClouds"].toArray();
        for(int i = 0; i < pointClouds.size(); i++){
            pointCloudFromJSON(pointClouds[i].toObject());
        }

        return;
    }
};

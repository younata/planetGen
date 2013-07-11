//
//  CelestialBody.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 1/17/13.
//
//

#include "CelestialBody.h"

#include "Orbit.h"

double uniformRand()
{
    double r = random();
    return r / ((double)RAND_MAX + 1);
}

double randBetween(double lower, double higher)
{
    double r = uniformRand();
    double a = higher-lower;
    return (a * r) + lower;
}

CelestialBody::CelestialBody(Orbit *o, Node *node, double mass, std::string name) :
SpaceObject(o, node, mass, name)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

CelestialBody::CelestialBody(Orbit *o, double mass, std::string name) :
SpaceObject(o, mass, name)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

CelestialBody::CelestialBody(Orbit *o, double mass) :
SpaceObject(o, mass)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

CelestialBody::CelestialBody(double mass, std::string name) :
SpaceObject(mass, name)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

CelestialBody::CelestialBody(double mass) :
SpaceObject(mass)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

CelestialBody::CelestialBody(Orbit *o, double mass, double radius, double dl, long ts) :
SpaceObject(o, mass),
radius(radius),
dayLength(dl),
terrainSeed(ts),
oblateness(0),
heightFactor(0.0014)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    
    maxTriangles = MAX_TRIANGLES;
    maxVertices = MAX_VERTICES;
    maxPoints = MAX_POINTS_;
}

void CelestialBody::genSphere(double mve, double minve)
{
    terrainGenerator = new CFractal(3, terrainSeed, 1.25, 2.005);
    maxVisibleError = mve;
    minVisibleError = minve;
    sphere = new RoamSphere(mve, minve, 128, radius, 0, this);
    sphere->maxTriangles = maxTriangles;
    sphere->center = position;
    sphere->cameraNode = cameraNode;
}

double CelestialBody::hillSphere()
{
    double a, e, M;
    a = orbit->semiMajorAxis();
    e = orbit->eccentricity();
    M = mass;
    return a * (1-e) * pow(mass / (3*M), 1.0/3.0);
}

void CelestialBody::setColors(std::vector<Color3> c)
{
    colors = c;
    if (sphere != NULL) {
        sphere->rm();
        genSphere(maxVisibleError, minVisibleError);
        sphere->maxTriangles = maxTriangles;
        singleThreadedUpdateMesh(cameraNode->getTranslationWorld(), Vector3(0,0,0));
    }
}

void CelestialBody::setTriangles(unsigned long t)
{
    maxTriangles = t;
    maxVertices = t*3;
    maxPoints = t*18;
    if (sphere != NULL) {
        sphere->rm();
        genSphere(maxVisibleError, minVisibleError);
        sphere->maxTriangles = maxTriangles;
        singleThreadedUpdateMesh(cameraNode->getTranslationWorld(), Vector3(0,0,0));
    }
    if (object ==  NULL)
        return;
    Model *model = object->getModel();
    object->setModel(NULL);
    SAFE_RELEASE(model);
}

sterrain CelestialBody::terrainAlgorithm(Vector3 nearestPosition, float octaves)
{
    // nearestPosition is in a line from the center through the surface.
    // return the terrain at the point where that line intersects with the surface.

    Vector3 n = Vector3();
    nearestPosition.normalize(&n);

    float f[3];
    f[0] = n.x;
    f[1] = n.y;
    f[2] = n.z;

    float height = terrainGenerator->fBmTest(f, octaves);
    assert(height != NAN);
    if (!ocean && height <= 0)
        height = 0;
    //height = 0;
    
    n.scale(1+(height*heightFactor));
    
    sterrain foo(n);
    
    float latitude = abs(f[1]);
    latitude += height*0.9;
    latitude *= 0.95;
    
    Color3 c = Color3(1,1,1);
    
    if (height > 0.0) {
        int totalColors = colors.size();
        std::vector<float>r, g, b;
        int count = 0;
        for (std::vector<Color3>::iterator i = colors.begin(); i != colors.end(); i++) {
            Color3 t = *i;
            if (ocean && count == totalColors-1)
                break;
            r.push_back(t.r);
            g.push_back(t.g);
            b.push_back(t.b);
            count++;
        }
        
        c.r = CMath::spline(latitude, r);
        c.g = CMath::spline(latitude, g);
        c.b = CMath::spline(latitude, b);
        
        ///*
        if (latitude < 0.75) {
            Color3 x = Color3(0.5, 0.175, 0.5);
            float y = 0.02 * terrainGenerator->RidgedMultifractal(f, octaves, 0.75, 0.25);
            c.r += (x.r*y);
            c.g += (x.g*y);
            c.b += (x.b*y);
        }//*/
    } else {
        if (colors.size() != 0)
            c = colors.back();//Color3(0.12, 0.3, 0.6);
        else
            c = Color3(0.12, 0.3, 0.6);
        if (iceCaps > 0 && latitude > iceCaps)
            c = Color3(1,1,1);
        else {
            float chaos = height;
            chaos = CMath::Max(chaos, -0.5f);
            c.r *= (1+chaos);
            c.g *= (1+chaos);
            c.b *= (1+chaos);
        }
            
    }
    
    foo.color = c;
    
    //foo.error =
    
    return foo;
}

std::vector<float> CelestialBody::vertices(Vector3 cameraPosition, Vector3 lookAt)
{
    /*
    if (currentScale == far || currentScale == parsec)
        return std::vector<float>();
     */

    Vector3 p = Vector3(position);
    double scaleFactor = getScale();
    p.scale(scaleFactor);
    
    double r = 1.0 / radius;
    
    cameraPosition.scale(r);
    
    if (sphere->isPlanetNull()) {
        sphere->setPlanet(this); // Shouldn't happen
        sphere->center = position;
        updateMesh(cameraPosition, lookAt);
        return std::vector<float>();
    }

    std::list<sterrain> *v = sphere->getVertices();
    std::vector<float> ret = std::vector<float>();
    ret.resize(maxPoints);
    unsigned long j = 0;
    
    scaleFactor = radius;

    int amount = 0;
    for (std::list<sterrain>::iterator i = v->begin(); i != v->end(); i++) {
        amount++;
        sterrain t = *i;
        Vector3 n = Vector3(t.x, t.y, t.z);
        Vector3 x = Vector3(n);
        x.scale(scaleFactor);
        ret.push_back(x.x);
        ret.push_back(x.y);
        ret.push_back(x.z);
        ret.push_back(t.color.r);
        ret.push_back(t.color.g);
        ret.push_back(t.color.b);
    }
    assert(amount != 0);
    return ret;
}

void *updateThread(void *ptr)
{
    CelestialBody *cb = (CelestialBody *)ptr;
    while (cb->continueRunningThread) {
        sem_wait(cb->updateSem);
        cb->actualUpdateMesh();
        cb->updateRequests->clear();
        sem_post(cb->updateSem);
    }
    return NULL;
}

void CelestialBody::endAllUpdates()
{
    continueRunningThread = false;
    pthread_join(updateThread_t, NULL);
    
    sem_wait(updateSem);
    sem_close(updateSem);
    sem_wait(meshSem);
    sem_close(meshSem);
    
    char name[128];
    sprintf(name, "%ld-update", terrainSeed);
    sem_unlink(name);
    sprintf(name, "%ld-mesh", terrainSeed);
    sem_unlink(name);
}

int createPThread(CelestialBody *cb);

int createPThread(CelestialBody *cb)
{
    return pthread_create(&(cb->updateThread_t), NULL, updateThread, (void*)cb);
}

void CelestialBody::singleThreadedUpdateMesh(Vector3 cameraPosition, Vector3 lookAt)
{
    Frustum f = cameraNode->getCamera()->getFrustum();
    
    if (object == NULL)
        object = gameplay::Node::create();
    
    gameplay::Model *model = object->getModel();
    gameplay::Mesh *mesh = NULL;
    
    if (model != NULL) {
        mesh = model->getMesh();
    }
    
    
    if (mesh == NULL) {
        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::COLOR, 3)
        };
        mesh = Mesh::createMesh(VertexFormat(elements, 2), maxPoints*2, true);
    }
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    
    if (model == NULL) {
        model = gameplay::Model::create(mesh);
        model->setMaterial("grid.material");
        object->setModel(model);
    }
    
    bool changed = false;
    sphere->center = object->getTranslationWorld();
    double scaleFactor = getScale();
#ifdef DEBUG
    scaleFactor = 1.0;
#endif
    scaleFactor *= radius;
    changed = sphere->update(cameraPosition / scaleFactor, lookAt, f);
    
    if (!changed)
        return;
    std::vector<float> v = vertices(cameraPosition, lookAt);
    verts = v;
    amountTriangles = verts.size()/18;
    
    mesh->setVertexData(&verts[0], 0, verts.size()/6);

}

void CelestialBody::updateMesh(Vector3 cameraPosition, Vector3 lookAt)
{
    camPosAndFrustum f;
    f.cameraPosition = cameraPosition;
    f.lookAt = lookAt;
    f.frustum = cameraNode->getCamera()->getFrustum();
    if (updateRequests == NULL)
        updateRequests = new std::list<camPosAndFrustum>();
    if (updateThread_t == 0) {
        if (createPThread(this) != 0)
            printf("error creating thread\n");
    }
    if (updateSem == NULL) {
        char name[128];
        sprintf(name, "%ld-update", terrainSeed);
        updateSem = sem_open(name, O_CREAT, 0, 0);
        sem_post(updateSem);
    }
    if (meshSem == NULL) {
        char name[128];
        sprintf(name, "%ld-mesh", terrainSeed);
        meshSem = sem_open(name, O_CREAT, 0, 0);
        sem_post(meshSem);
    }
    
    if (object == NULL)
        object = gameplay::Node::create();
    
    gameplay::Model *model = object->getModel();
    gameplay::Mesh *mesh = NULL;
    
    if (model != NULL) {
        mesh = model->getMesh();
    }
    
    
    if (mesh == NULL) {
        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::COLOR, 3)
        };
        mesh = Mesh::createMesh(VertexFormat(elements, 2), maxPoints*2, true);
    }
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    
    if (model == NULL) {
        model = gameplay::Model::create(mesh);
        model->setMaterial("grid.material");
        object->setModel(model);
    }
    
    sem_wait(updateSem);
    updateRequests->push_back(f);
    sem_post(updateSem);
    
    sem_wait(meshSem);
    if (verts.size() == 0) {
        sem_post(meshSem);
        return;
    }
    mesh->setVertexData(&verts[0], 0, verts.size()/6);
    sem_post(meshSem);
}

void CelestialBody::actualUpdateMesh()
{
    if (updateRequests == NULL)
        return;
    if (updateRequests->size() == 0)
        return;
    camPosAndFrustum f = updateRequests->back();
    Vector3 cameraPosition = f.cameraPosition;
    Vector3 lookAt = f.lookAt;
    Frustum frustum = f.frustum;
    
    bool changed = false;
    sphere->center = object->getTranslationWorld();
    double scaleFactor = getScale();
#ifdef DEBUG
    scaleFactor = 1.0;
#endif
    scaleFactor *= radius;
    changed = sphere->update(cameraPosition / scaleFactor, lookAt, frustum);

    
    gameplay::Model *model = object->getModel();
    gameplay::Mesh *mesh = NULL;
    
    if (model != NULL) {
        mesh = model->getMesh();
    }
    if (model == NULL || mesh == NULL)
        return;
    
    if (!changed)
        return;
    std::vector<float> v = vertices(cameraPosition, lookAt);
    sem_wait(meshSem);
    verts = v;
    amountTriangles = verts.size()/18;
    sem_post(meshSem);
}

int CelestialBody::amountOfTriangles() { return sphere->triangles->size(); }

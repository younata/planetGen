//
//  CelestialBody.h
//  SpaceSim
//
//  Created by Rachel Brindle on 1/17/13.
//
//

#ifndef __SpaceSim__CelestialBody__
#define __SpaceSim__CelestialBody__

#include <iostream>
#include <string>

#include <list>
#include <vector>
#include "sterrain.h"
#include "noise.h"
#include "roam.h"

#include "gameplay.h"

#include <pthread.h>
#include <semaphore.h>

#include "SpaceObject.h"

#define MAX_POINTS_ 262080
#define MAX_VERTICES (MAX_POINTS_ / 6)
#define MAX_TRIANGLES (MAX_VERTICES / 3)
// 14560 = MAX_TRIANGLES

double uniformRand();
double randBetween(double lower, double higher);

using namespace gameplay;

void *updateThread(void *ptr);

struct camPosAndFrustum {
    Vector3 cameraPosition;
    Vector3 lookAt;
    Frustum frustum;
};

class CelestialBody : public SpaceObject {
public:
    double dayLength;
    double oblateness;
    long terrainSeed;
    CFractal *terrainGenerator;
    
    double minVisibleError;
    double maxVisibleError;
    
    unsigned long maxPoints;
    unsigned long maxVertices;
    unsigned long maxTriangles;
    
    sem_t *updateSem = NULL, *meshSem = NULL;
    pthread_t updateThread_t = 0;
    
    bool continueRunningThread = true;
    
    std::vector<float>verts;
    
    std::list<camPosAndFrustum> *updateRequests = NULL;
    
    void actualUpdateMesh();
    void singleThreadedUpdateMesh(Vector3 cameraPosition, Vector3 lookAt);
    
    void endAllUpdates();
    
    std::vector<Color3> colors;

    RoamSphere *sphere = NULL;
        
    unsigned long amountTriangles;
    
    bool atmosphere = true;
    bool ocean = true;
    float iceCaps = 0.0;
    
    double radius;

    void genSphere(double mve=0.001, double minve=0.0005);
    double hillSphere();
    double heightFactor; // defaults to 0.0014, for refence. The height of the tallest mountain, in units of planetary radius.

    void setColors(std::vector<Color3> c);

    void setTriangles(unsigned long t);

    sterrain terrainAlgorithm(Vector3 nearestPosition, float octaves);
    std::vector<float> vertices(Vector3 cameraPosition, Vector3 lookAt);
    void updateMesh(Vector3 cameraPosition, Vector3 lookAt);
    
    // Either the stellar (http://en.wikipedia.org/wiki/Stellar_classification)
    // or planetary class (uses the Star Trek planetary classifications)
    // (http://en.wikipedia.org/wiki/Class_M_planet)

    int amountOfTriangles();
        
    CelestialBody(Orbit *o, Node *node, double mass, std::string name);
    CelestialBody(Orbit *o, double mass, std::string name);
    CelestialBody(double mass, std::string name);
    CelestialBody(Orbit *o, double mass);
    CelestialBody(Orbit *o, double mass, double r, double dl, long ts);
    CelestialBody(double mass);
};

#endif /* defined(__SpaceSim__CelestialBody__) */

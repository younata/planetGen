#ifndef __ROAM_H__
#define __ROAM_H__

#include <list>

#include "gameplay.h"

#include "sterrain.h"

class CelestialBody;

class RoamTriangle; // in case the compiler isn't a multipass one.
class RoamDiamond;

float sqrt_approx(float z);

using namespace gameplay;

bool frustrumContains(Frustum f, Vector3 p);
bool angle3Points(Vector3 a, Vector3 b, Vector3 c);

// My implementation of ROAM is probably not optimal.
// However, at this point, it's too early to say what parts should be optimized.
class RoamSphere
{
public:
    unsigned long id;
    unsigned long did;
    
    unsigned long maxTriangles;
    
    Vector3 center;
    
    std::list<RoamTriangle *> *triangles;
    std::list<RoamTriangle *> *splitQueue;
    std::list<RoamTriangle *> *mergeQueue;

    double maxVisualError;
    double minVisualError;
    
    Node *cameraNode;

    double radius;
    double oblateness; // 1 / number; this is assumed to be 0.
                       // earth is roughly 1/300
    // TODO: add oblateness. (this might be something to add to terrainAlgorithm)

    CelestialBody *body;

    int maxDepth;

    RoamSphere(double mve, double minve, int md, double r, double o, CelestialBody *b);

    void setPlanet(CelestialBody *b);
    bool isPlanetNull();

    bool CollisionCheck(Vector3 camera);

    /*
     * Updates the mesh.
     * Returns whether there was any change at all.
     */
    bool update(Vector3 cp, Vector3 la, Frustum frustum); // cameraPosition, lookAt
    
    bool confirmAllVertices();
        
    void split(RoamTriangle *t);
    void merge(RoamDiamond *d);
    
    int checkForReferences(RoamTriangle *t, bool print);

    std::list<sterrain> *getVertices();
    
    void rm();

    void removeDiamond(RoamDiamond *d);
    RoamDiamond *createDiamond(RoamTriangle *t);

};

class RoamTriangle
{
public:
    double error;

    sterrain vertexes[3]; // vertex 0 is opposite of edge 0,
                         // vertex 1 is opposite of edge 1,
                         // vertex 2 is opposite of edge 2,

    sterrain midpoint;
    Vector3 incenter;

    unsigned long id;

    RoamTriangle *parent;
    RoamTriangle *child;
    RoamDiamond *diamond;

    RoamTriangle *edge0; // longest side
    RoamTriangle *edge1; // right of edge 0
    RoamTriangle *edge2; // left of edge 0
    int size; // actually, inverse size

    inline bool operator==(const RoamTriangle& v) const;

    RoamTriangle(RoamTriangle *p, int s, unsigned long i); // parent

    void setEdges(RoamTriangle *e0, RoamTriangle *e1, RoamTriangle *e2);
    void setPosition(sterrain e0, sterrain e1, sterrain e2);

    void setMidpoint(sterrain m);

    double calculateVisualError(double distanceToCamera); // { return error / distanceToCamera; }
    float getPriority(Vector3 pos, Vector3 heading, float horizon);
    
    bool confirmVertex(RoamTriangle *t, int edge);
    
    bool isValidTriangle();

    bool equals(RoamTriangle *t);
    
    bool containsEdge();

    bool isInDiamond();
    bool isInDiamondWith(RoamTriangle *e1, RoamTriangle *e2, RoamTriangle *op);
};

class RoamDiamond
{
public:
    RoamTriangle *parent[2];
    RoamTriangle *child[2];

    unsigned long id;

    Vector3 midpoint;
    float offset;

    inline bool operator==(const RoamDiamond& v) const;

    RoamDiamond(RoamTriangle *p1, RoamTriangle *p2, RoamTriangle *c1, RoamTriangle *c2, unsigned long i);

    //float getPriority(Vector3 pos, Vector3 heading, float horizon);

    void rm();
};

#endif

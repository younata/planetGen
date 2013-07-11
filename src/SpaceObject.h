//
//  SpaceObject.h
//  SpaceSim
//
//  Created by Rachel Brindle on 3/27/13.
//
//

#ifndef __SpaceSim__SpaceObject__
#define __SpaceSim__SpaceObject__

#include <list>
#include <vector>
#include "gameplay.h"

#define G 6.67e-11

#define METERS_TO_AU 149597870700 // meters = 1 AU
#define AU_TO_PARSEC 206264.806 // AU = 1 parsec

#define METERS_TO_LIGHT_SECOND 299792458.0
#define SECONDS_TO_HOUR 3600.0
#define HOURS_TO_YEAR 8766.0

using namespace gameplay;

enum unitScale {
    meters, // default
    lightMicrosecond, // 299.792458 meters
    lightMillisecond, // 299,792.458 meters
    lightSecond, // 299,792,458 meters
    lightHour, // 3600 lightSeconds
    lightYear, // 8766 lightHours
    astronomicalUnits, // 149,597,870,700 meters
    parsec, // 206,264.806 au
    far // just stick it at the end of the Z buffer.
};
class Orbit;

class SpaceObject {
public:
    double scale;
    double orbitScale;
    double viewScale;
    
    Orbit *orbit = NULL;
    double mass = 1;
    Vector3 velocity;
    Vector3 position;
    
    Vector3 rotationalVelocity;
    
    std::string name = "untitled";
    
    Node *object = NULL;
    Node *cameraNode = NULL;
    
    virtual unitScale defaultScale() { return far; }
    virtual void tick(double dt, double seconds) { return; }
    
    double gravParam();
    
    Orbit *getOrbit();
    void setOrbit(Orbit *orbit);
    
    Vector3 getVelocity();
    Vector3 getPosition();
    
    Node *getNode();
    void setNode(Node *node);
    
    std::string getName();
    
    double getScale();
    void setScale(double s);
    
    SpaceObject(Orbit *o, Node *node, double mass, std::string name);
    SpaceObject(Orbit *o, double mass, std::string name);
    SpaceObject(double mass, std::string name);
    SpaceObject(Orbit *o, double mass);
    SpaceObject(std::string name);
    SpaceObject(double mass);
};

#endif /* defined(__SpaceSim__SpaceObject__) */

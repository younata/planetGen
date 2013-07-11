//
//  Orbit.h
//  SpaceSim
//
//  Created by Rachel Brindle on 1/17/13.
//
//

#ifndef __SpaceSim__Orbit__
#define __SpaceSim__Orbit__

#include <iostream>
#include "gameplay.h"

using namespace gameplay;

class SpaceObject;

class Orbit
{
protected:
    double a, e, i, l, w, t;
    
    Vector3 epoch;
    SpaceObject *ref;
    
public:
    double gravParam;
    
    Orbit();
    Orbit(SpaceObject *ref, double sa, double se, double si, double sl, double sw, double st);
    Orbit(SpaceObject *ref, Vector3 r, Vector3 v);
    
    SpaceObject *reference();
    double semiMajorAxis();
    double eccentricity();
    double inclination();
    double longitudeOfAscendingNode();
    double argumentOfPeriapsis();
    double trueAnomaly();
    
    double semiMinorAxis();
    double period();
    double eccentricAnomaly();
    double meanAnomaly();
    double meanMotion();
    double timeSincePeriapsis();
    double semiparameter();
    Vector3 position();
    Vector3 velocity();
    
    Orbit *copy();
    
    
    // vectors in geocentric equatorial inertial coordinates
    void calcFromPosVel(Vector3 r, Vector3 v);
    double estimateTrueAnomaly(double meanAnomaly);
    double calcEccentricAnomaly(double meanAnomaly);
    void calcTrueAnomaly(double eccentricAnomaly);
    
    void step(double time);
};

#endif /* defined(__SpaceSim__Orbit__) */

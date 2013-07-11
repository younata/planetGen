#ifndef __PLANET_H__
#define __PLANET_H__

#include "CelestialBody.h"

class planet : public CelestialBody {
public:
    std::list<planet *> *moons;
        
    planet(double m, Orbit *orb, double r, double dl, double o, long ts);
    
    void tick(double dt, double seconds);

    unsigned long count();
    
    unitScale defaultScale();

    sterrain terrainAlgorithm(Vector3 nearestPosition, float octaves);

    bool generateMoon(int i, double C, double t_zero, double hs);
    void generateMoons(int amount, long seed);
};

#endif

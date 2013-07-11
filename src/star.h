#ifndef __STAR_H__
#define __STAR_H__

#include <list>
#include "orbit.h"
#include "CelestialBody.h"

class planet;

enum starClass {
    OClass = 0,
    BClass,
    AClass,
    FClass,
    GClass,
    KClass,
    MClass
};

enum planetaryClass {
    ClassD = 0, // Moons
    ClassH, // Desert
    ClassI, // Gas Giant, Small
    ClassJ, // Gas Giant, Large
    ClassK, // Earth mass, no atmo.
    ClassL, // uh...
    ClassM, // Earth
    ClassN, // CO2, no O2, higher water
    ClassO, // Water
    ClassP, // Water-ice
    ClassS, // Gas Giant, Medium
    ClassT, // Gas Giant, Extra Large
    ClassY  // Venus
};

class Universe;

class star : public CelestialBody {
    double currentAge;
    
    double habitableZoneOuter;
    double habitableZoneInner;
    
public:
    std::list<planet *> *planets;
    Universe *universe;
    
    Scene *scene = NULL;

    double luminosity();
    double apparentBrightness(double distance);
    double surfaceTemp();
    double lifetime();
    double remainingLifetime();
    void addPlanet(planet *p) { planets->push_back(p); }

    unsigned long countPlanets();
    
    unitScale defaultScale();

    void generatePlanet(int i, double auConversion);
    void generatePlanets(int amount, long seed);
    
    void tick(double dt, double seconds);

    star(double m, double a);
};

#endif

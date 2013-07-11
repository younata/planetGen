#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include "planet.h"
#include "orbitingCamera.h"
#include "star.h"

#include <map>

#define MAX_RENDER_DISTANCE 100000.0 // 100,000 units.
// from 100,000 on, it scales exponentially
// such that 100,001 has scale of 0.99,
// and 200,000 has a scale of 0.9 or so.
// 300,000 = 0.85
// 400,00 = 8

typedef std::list<star*> starList;
typedef std::list<planet*> planetList;

typedef std::vector<Color3> colorList;

class Universe {
public:
    std::list<star*> *stars;
    CelestialBody *currentFocus;
    
    Scene *scene;
    
    std::map<planetaryClass, std::list<colorList*> > planetaryColors;
    std::map<starClass, std::list<colorList*> > starColors;
    
    void readClasses();
    
    unsigned int seed;
        
    OrbitingCamera *camera;
        
    unitScale currentScale;
    
    void generateStar();
    
    void changeFocus(CelestialBody *newFocus);
        
    Universe(unsigned long amountStars, unsigned int seed, OrbitingCamera *camera, Scene *scene);
    
    static double scale(unitScale currentScale);

    unsigned long totalBodies();
    
    bool render(CelestialBody *cb);
    
    void tick(double dt);
    
    void draw();
    bool draw(CelestialBody *cb, double scaleFactor, Vector3 cp);
};

#endif

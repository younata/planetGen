//
//  World.h
//  SpaceSim
//
//  Created by Rachel Brindle on 2/22/13.
//
//

#ifndef __SpaceSim__World__
#define __SpaceSim__World__

#include <iostream>
#include "noise.h"
#include "planet.h"
#include "roam.h"
#include "sterrain.h"
#include "orbitingCamera.h"
#include "universe.h"
#include "star.h"

class DemoWorld {
public:
    unsigned int seed;
    planet *earth;
    star *sun;
};

#endif /* defined(__SpaceSim__World__) */
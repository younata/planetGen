//
//  simpleCamera.h
//  SpaceSim
//
//  Created by Rachel Brindle on 1/13/13.
//
//

#ifndef __SpaceSim__simpleCamera__
#define __SpaceSim__simpleCamera__

#include <iostream>

#include "gameplay.h"

gameplay::Quaternion *createQuaternionFromEuler(gameplay::Vector3 *euler);

class simpleCamera {
    gameplay::Vector3 target;
    gameplay::Vector3 position;
    
    gameplay::Camera *c;
    
    bool recalculateView = false;
    
public:
    void setLookAt(gameplay::Vector3 target);
    void setLookAt(double x, double y, double z);
    
    void setPosition(gameplay::Vector3 position);
    void setPosition(double x, double y, double z);
    
    void calculateViewMatrix();
    
    void setCamera(gameplay::Camera *c);
    gameplay::Camera *getCamera();
    
    gameplay::Vector3 getLookAt();
    gameplay::Vector3 getPosition();
};

#endif /* defined(__SpaceSim__simpleCamera__) */

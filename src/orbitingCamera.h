//
//  orbitingCamera.h
//  SpaceSim
//
//  Created by Rachel Brindle on 1/13/13.
//
//

#ifndef __SpaceSim__orbitingCamera__
#define __SpaceSim__orbitingCamera__

#include <iostream>
#include <vector>

#include "gameplay.h"
#include "simpleCamera.h"

class TouchWrapper
{
public:
    unsigned int hash;
    gameplay::Vector2 location;
    gameplay::Vector2 previousLocation;
    
    inline bool operator==(const TouchWrapper& t);
    inline bool operator!=(const TouchWrapper& t);
    
    void touchMoved(gameplay::Vector2 newLocation);
    TouchWrapper(int x, int y, unsigned int hash);
    TouchWrapper(gameplay::Vector2 location, unsigned int hash);
};

class MouseWrapper
{
public:
    gameplay::Vector2 location;
    gameplay::Vector2 previousLocation;
    int button;
    
    inline bool operator==(const MouseWrapper& t);
    inline bool operator!=(const MouseWrapper& t);
    
    void mouseMoved(gameplay::Vector2 newLocation);
    MouseWrapper(int x, int y, int mouseButton);
    MouseWrapper(gameplay::Vector2 location, int mouseButton);
};

class OrbitingCamera
{
    simpleCamera *c;
    
    gameplay::Vector3 target;
    
    std::vector<TouchWrapper> *touches;
    MouseWrapper *mouse;
    
    int width, height;
    
    bool orbitChanged = false;
    
public:
    double orbit;
    double orbitMin;
    double orbitMax;
    double vTheta;
    double vPhi;
    double theta;
    double phi;
    double moveSpeed;
    double damping;
    bool doubleTapEnabled;
    double zoomSpeed;
    
    double scaleFactor;
    
    void setCamera(simpleCamera *c);
    simpleCamera *getCamera();
    void setTarget(gameplay::Node *t);
    void setTarget(gameplay::Vector3 t);
    void setTarget(float x, float y, float z);
        
    void update();
    void reset();
    
    void mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, int wheelDelta);
    void touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
    
    void mouseBegan();
    void mouseMoved();
    void touchesBegan();
    void touchesMoved();
    
    void setDisplayStuff(int width, int height);
    
    double distanceBetweenTwoPoints(gameplay::Vector2 one, gameplay::Vector2 two);
};

#endif /* defined(__SpaceSim__orbitingCamera__) */

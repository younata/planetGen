//
//  orbitingCamera.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 1/13/13.
//
//

#include "orbitingCamera.h"

#define nil NULL

using namespace gameplay;

// TouchWrapper
TouchWrapper::TouchWrapper(Vector2 location, unsigned int hash) :
location(location),
hash(hash),
previousLocation(location)
{}

TouchWrapper::TouchWrapper(int x, int y, unsigned int hash) :
hash(hash),
previousLocation(x,y),
location(x,y)
{}

void TouchWrapper::touchMoved(Vector2 newLocation)
{
    previousLocation = location;
    location = newLocation;
}

inline bool TouchWrapper::operator==(const TouchWrapper& t)
{
    return hash == t.hash;
}

inline bool TouchWrapper::operator!=(const TouchWrapper& t)
{
    return hash != t.hash;
}

// MouseWrapper
MouseWrapper::MouseWrapper(Vector2 location, int mouseButton) :
location(location),
button(mouseButton),
previousLocation(location)
{}

MouseWrapper::MouseWrapper(int x, int y, int mouseButton) :
button(mouseButton),
previousLocation(x,y),
location(x,y)
{}

void MouseWrapper::mouseMoved(Vector2 newLocation)
{
    previousLocation = location;
    location = newLocation;
}

inline bool MouseWrapper::operator==(const MouseWrapper& t)
{
    return button == t.button;
}

inline bool MouseWrapper::operator!=(const MouseWrapper& t)
{
    return button != t.button;
}

// OrbitingCamera

void OrbitingCamera::setDisplayStuff(int width, int height)
{
    this->width = width;
    this->height = height;
}

void OrbitingCamera::setCamera(simpleCamera *c)
{
    this->c = c;
}

simpleCamera *OrbitingCamera::getCamera()
{
    return this->c;
}

void OrbitingCamera::setTarget(Node *t)
{
    setTarget(t->getTranslationWorld());
}

void OrbitingCamera::setTarget(float x, float y, float z)
{
    setTarget(Vector3(x,y,z));
}

void OrbitingCamera::setTarget(Vector3 t)
{
    if (c != NULL)
        c->setLookAt(t);
    
    target = t;
    orbitChanged = true;
}

void OrbitingCamera::reset()
{
    this->orbit = 10;
    this->orbitMin = 1;
    this->orbitMax = 1.0e12;
    this->theta = 0;
    this->phi = 0;
    this->vTheta = 0;
    this->vPhi = 0;
    this->damping = 0;
    this->zoomSpeed = 1.0 / 128.0;
    this->moveSpeed = 10.0;
        
    this->scaleFactor = 1.0;
    
    this->doubleTapEnabled = true;
    if (this->c != NULL) {
        this->setTarget(Vector3(0,0,0));
    }
}

void OrbitingCamera::update()
{
    if (c == NULL)
        return;
    
    if (orbitChanged) {
        theta -= vTheta;
        phi -= vPhi;
        
        if (phi >= M_PI_2)
            phi = 89.9 / 180.0 * M_PI;
        if (phi <= -1 * M_PI_2)
            phi = -89.9 / 180.0 * M_PI;
        
        float y = orbit * sinf(phi);// * M_PI / 180);
        float l = orbit * cosf(phi);// * M_PI / 180);
        float x = l * sinf(theta);// * M_PI / 180);
        float z = l * cosf(theta);// * M_PI / 180);
                
        c->setPosition(Vector3(x,y,z) + c->getLookAt());

        vTheta *= damping;
        vPhi *= damping;
        
        Vector3 p = c->getPosition();
        Vector3 t = c->getLookAt();
        Vector3 n = t - p;
                
        zoomSpeed = (orbit/(orbitMin*128)) / scaleFactor;
        //printf("%f\n", zoomSpeed);
        
        orbitChanged = false;
    }
    
    c->calculateViewMatrix();
}

void OrbitingCamera::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    switch (evt) {
        case Mouse::MOUSE_MOVE:
            if (mouse == NULL)
                break;
            mouse->mouseMoved(Vector2(x,y));
            mouseMoved();
            break;
        case Mouse::MOUSE_PRESS_LEFT_BUTTON:
            if (mouse == NULL)
                mouse = new MouseWrapper(x,y,0);
            break;
        case Mouse::MOUSE_RELEASE_LEFT_BUTTON:
            if (mouse != NULL) {
                delete mouse;
                mouse = NULL;
            }
            break;
        case Mouse::MOUSE_WHEEL: {            
            float dx = wheelDelta;
            this->orbit += dx * zoomSpeed;
            
            if (this->orbit <= this->orbitMin)
                this->orbit = this->orbitMin;
            else if (this->orbit >= this->orbitMax)
                this->orbit = this->orbitMax;
            
            orbitChanged = true;
            break;
        } default:
            break;
    }
}

void OrbitingCamera::mouseBegan()
{
    vTheta = 0;
    vPhi = 0;
}

void OrbitingCamera::mouseMoved()
{
    Vector2 l = mouse->location;
    Vector2 p = mouse->previousLocation;
    vPhi = -1 * (l.y - p.y) / height * moveSpeed * M_PI_2;
    vTheta = (l.x - p.x) / width * moveSpeed * M_PI_2;
    orbitChanged = true;
}

void OrbitingCamera::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    TouchWrapper t = TouchWrapper(x,y,contactIndex);
    switch (evt)
    {
        case Touch::TOUCH_PRESS: {
            if (touches == NULL)
                touches = new std::vector<TouchWrapper>();
            touches->push_back(t);
            touchesBegan();
            break;
        }
        case Touch::TOUCH_RELEASE:
            for (std::vector<TouchWrapper>::iterator i = touches->begin(); i < touches->end(); i++) {
                TouchWrapper tw = *i;
                if (tw == t) {
                    touches->erase(i);
                }
            }
            break;
        case Touch::TOUCH_MOVE: {
            for (std::vector<TouchWrapper>::iterator i = touches->begin(); i < touches->end(); i++) {
                TouchWrapper tw = *i;
                if (tw == t) {
                    tw.touchMoved(Vector2(x,y));
                    touches->erase(i);
                    touches->insert(i, tw);
                }
            }
            touchesMoved();
            break;
        }
    };
}

void OrbitingCamera::touchesBegan()
{
    if (touches->size() == 1) {
        vTheta = 0;
        vPhi = 0;
    }
}

void OrbitingCamera::touchesMoved()
{
    if (touches->size() == 1) {
        TouchWrapper t = touches->front();
        Vector2 l = t.location;
        Vector2 p = t.previousLocation;
        //assert(l != NULL);
        //assert(l != p);
        vPhi = -1 * (l.y - p.y) / height * moveSpeed * M_PI_2;
        vTheta = (l.x - p.x) / width * moveSpeed * M_PI_2;
        orbitChanged = true;
    } else if (touches->size() == 2) {
        TouchWrapper t1 = touches->front();
        TouchWrapper t2 = touches->back();
        
        //assert(t1.location != NULL);
        //assert(t1.previousLocation != NULL);
        //assert(t2.location != NULL);
        //assert(t2.location != NULL);
        assert(t1 != t2);
        
        float pd = distanceBetweenTwoPoints(t1.previousLocation, t2.previousLocation);
        float cd = distanceBetweenTwoPoints(t1.location, t2.location);
        
        float dx = pd - cd;
        this->orbit += dx * zoomSpeed;
        
        if (this->orbit <= this->orbitMin)
            this->orbit = this->orbitMin;
        else if (this->orbit >= this->orbitMax)
            this->orbit = this->orbitMax;
        
        orbitChanged = true;
    }
}

double OrbitingCamera::distanceBetweenTwoPoints(Vector2 one, Vector2 two)
{
    double dx = one.x - two.x;
	double dy = one.y - two.y;
	
	return sqrt(dx*dx + dy*dy);
}
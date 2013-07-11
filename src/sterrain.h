#ifndef __STERRAIN_H__
#define __STERRAIN_H__

#include "gameplay.h"

struct Color3
{
    float r;
    float g;
    float b;
    Color3(float r, float g, float b) : r(r), g(g), b(b) {}
};

class sterrain : public gameplay::Vector3
{
public:
    Color3 color;
    float error; // Error from what it should be.

    sterrain(float x, float y, float z) : Vector3(x,y,z), color(1.0, 1.0, 1.0) {}
    sterrain(Vector3 n) : Vector3(n), color(1.0, 1.0, 1.0) {}
    sterrain() : color(1.0, 1.0, 1.0) {}
};

#endif

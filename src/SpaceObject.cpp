//
//  SpaceObject.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 3/27/13.
//
//

#include "SpaceObject.h"
#include "Orbit.h"

SpaceObject::SpaceObject(Orbit *o, Node *node, double mass, std::string name) : orbit(o), object(node), mass(mass), name(name)
{}

SpaceObject::SpaceObject(Orbit *o, double mass, std::string name) : orbit(o), mass(mass), name(name)
{}

SpaceObject::SpaceObject(double mass, std::string name) : mass(mass), name(name)
{}

SpaceObject::SpaceObject(Orbit *o, double mass) : orbit(o), mass(mass)
{}

SpaceObject::SpaceObject(std::string name) : name(name)
{}

SpaceObject::SpaceObject(double mass) : mass(mass)
{}

Orbit *SpaceObject::getOrbit()
{
    return orbit;
}

void SpaceObject::setOrbit(Orbit *orbit)
{
    this->orbit = orbit;
}

Vector3 SpaceObject::getVelocity()
{
    if (orbit == NULL)
        return Vector3();
    velocity = orbit->velocity();
    return orbit->velocity();
}

Vector3 SpaceObject::getPosition()
{
    if (orbit == NULL)
        return Vector3();
    double scaleFactor = getScale();
    position = orbit->position() * scaleFactor;
    return orbit->position() * scaleFactor;
}

double SpaceObject::gravParam()
{
    return mass * G;
}

Node *SpaceObject::getNode()
{
    return object;
}

void SpaceObject::setNode(Node *node)
{
    object = node;
}

std::string SpaceObject::getName()
{
    return name;
}

double SpaceObject::getScale()
{
    return scale;
}

void SpaceObject::setScale(double s)
{
    scale = s;
}
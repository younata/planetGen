//
//  simpleCamera.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 1/13/13.
//
//

#include "simpleCamera.h"

// wrapper stuff

gameplay::Quaternion *createQuaternionFromEuler(gameplay::Vector3 *euler)
{
    euler->normalize();
    
    double p = euler->x * (M_PI / 180) / 2.0;
    double yn = euler->y * (M_PI / 180) / 2.0;
    double r = euler->z * (M_PI / 180) / 2.0;
    
    float sinp = sin(p);
    float siny = sin(yn);
    float sinr = sin(r);
    float cosp = cos(p);
    float cosy = cos(yn);
    float cosr = cos(r);
    
    float x = sinr * cosp * cosy - cosr * sinp * siny;
    float y = cosr * sinp * cosy + sinr * cosp * siny;
    float z = cosr * cosp * siny - sinr * sinp * cosy;
    float w = cosr * cosp * cosy + sinr * sinp * siny;
    
    // normaliizing.
    double mag2 = w * w + x * x + y * y + z * z;
    double mag = sqrt(mag2);
    w /= mag;
    x /= mag;
    y /= mag;
    z /= mag;
    
    return new gameplay::Quaternion(x, y, z, w);
}

using namespace gameplay;

void simpleCamera::setLookAt(double x, double y, double z)
{
    this->setLookAt(Vector3(x,y,z));
}

void simpleCamera::setPosition(double x, double y, double z)
{
    this->setLookAt(Vector3(x,y,z));
}

void simpleCamera::setCamera(Camera *c)
{
    if (c == NULL) {
        fprintf(stderr, "simpleCamera:attempted to set camera to NULL\n");
        return;
    }
    this->c = c;
    this->position = c->getNode()->getTranslationWorld();
    recalculateView = true;
}

Camera *simpleCamera::getCamera()
{
    return this->c;
}

// the actual stuff

void simpleCamera::setLookAt(Vector3 target)
{
    assert(c != NULL);
    
    if (target != this->target) {
        recalculateView = true;
        this->target = target;
    }
}

void simpleCamera::setPosition(Vector3 position)
{
    assert(c != NULL);

    if (position != this->position) {
        recalculateView = true;
        this->position = position;
    }
}

void simpleCamera::calculateViewMatrix()
{
    if (!recalculateView)
        return;
    
    Node *cn = c->getNode();
    
    cn->setTranslation(position);
    
    
    /*
     _viewMatrix = [Isgl3dGLU lookAt:_cameraPosition.x eyey:_cameraPosition.y eyez:_cameraPosition.z
     centerx:_lookAt.x centery:_lookAt.y centerz:_lookAt.z
     upx:_up.x upy:_up.y upz:_up.z];
     
     createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ, float targetCenterX,
     float targetCenterY, float targetCenterZ, float upX, float upY, float upZ, Matrix* dst);
     */
    
    Vector3 wp = cn->getTranslationWorld();
    
    Matrix m = Matrix::identity();
    Matrix::createLookAt(wp, target, Vector3::unitY(), &m);
    //Matrix::createLookAt(wp.x, wp.y, wp.z, target.x, target.y, target.z, 0, 1, 0, &m);
    m.transpose();
    Quaternion q;
    m.getRotation(&q);
    cn->setRotation(q);
    
    recalculateView = false;
    
}

Vector3 simpleCamera::getLookAt()
{
    if (this->c == NULL)
        return Vector3();
    
//    this->c->getNode()->getForwardVectorView();
    return target;
}

Vector3 simpleCamera::getPosition()
{
    if (this->c == NULL)
        return Vector3();

    return this->c->getNode()->getTranslationWorld();
}
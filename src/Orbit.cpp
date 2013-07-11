//
//  Orbit.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 1/17/13.
//
//

#include "Orbit.h"

/*
 a = semiMajorAxis
 e = eccentricity
 T = timeAtPerihelian
 i = Inclination
 N = longitudeOfAscendingNode
 w = AngleFromAscendingNode
 */

#include "SpaceObject.h"


Orbit::Orbit() { }
Orbit::Orbit(SpaceObject *ref, double sa, double se, double si, double sl, double sw, double st) : ref(ref), a(sa), e(se), i(si), l(sl), w(sw), t(st) {}
Orbit::Orbit(SpaceObject *ref, Vector3 r, Vector3 v) : ref(ref)
{
    calcFromPosVel(r, v);
}

SpaceObject * Orbit::reference() { return ref; }
double Orbit::semiMajorAxis() { return a; }
double Orbit::eccentricity() { return e; }
double Orbit::inclination() { return i; }
double Orbit::longitudeOfAscendingNode() { return l; }
double Orbit::argumentOfPeriapsis() { return w; }
double Orbit::trueAnomaly() { return t; }

Orbit* Orbit::copy()
{
    return new Orbit(ref, a, e, i, l, w, t);
}


double Orbit::semiMinorAxis()
{
    return sqrt(a*a * (1 - e*e));
}

double Orbit::period()
{
    double u = ref->gravParam();
    return 2 * M_PI * sqrt(a*a*a / u);
}

double Orbit::eccentricAnomaly()
{
    double E = acos((e + cos(t)) / (1 + e * cos(t)));
    if (t > M_PI && E < M_PI)
        E = 2*M_PI - E;
    return E;
}

double Orbit::meanAnomaly()
{
    double E = eccentricAnomaly();
    double M = E - e * sin(E);
    if (E > M_PI && M < M_PI)
        M = 2*M_PI - M;
    return M;
}

double Orbit::meanMotion()
{
    double u = ref->gravParam();
    return sqrt(u / (a*a*a));
}

double Orbit::timeSincePeriapsis()
{
    return meanAnomaly() / meanMotion();
}

double Orbit::semiparameter()
{
    return a * (1 - e*e);
}

Vector3 Orbit::position()
{
    double p = semiparameter();
    Vector3 r;
    r.x = p * (cos(l) * cos(w + t) - sin(l) * cos(i) * sin(w + t));
    r.z = p * (sin(l) * cos(w + t) + cos(l) * cos(i) * sin(w + t));
    r.y = p * (sin(i) * sin(w + t));
    r = r - epoch;
    return r + ref->getPosition();
}

Vector3 Orbit::velocity()
{
    double p = semiparameter();
    double u = ref->gravParam();
    Vector3 v;
    double g = -sqrt(u/p);
    v.z = g * (cos(l)          * (sin(w + t) + e * sin(w)) +
               sin(l) * cos(i) * (cos(w + t) + e * cos(w)));
    v.y = g * (sin(l)          * (sin(w + t) + e * sin(w)) -
               cos(l) * cos(i) * (cos(w + t) + e * cos(w)));
    v.x = g * (sin(i) * (cos(w + t) + e * cos(w)));
    return v;
}

Vector3 cross(Vector3 a, Vector3 b);

Vector3 cross(Vector3 a, Vector3 b)
{
    Vector3 ret = Vector3(a);
    ret.cross(b);
    return ret;
}

// vectors in geocentric equatorial inertial coordinates
void Orbit::calcFromPosVel(Vector3 r, Vector3 v)
{
    // calculate specific relative angular momement
    Vector3 h = cross(r,v);
    
    // calculate vector to the ascending node
    Vector3 n(-h.y, h.x, 0);
    
    // standard gravity
    double u = ref->gravParam();
    
    // calculate eccentricity vector and scalar
    Vector3 e = (cross(v, h) * (1.0 / u)) - (r * (1.0 / r.length()));
    this->e = e.length();
    
    // calculate specific orbital energy and semi-major axis
    double E = v.lengthSquared() * 0.5 - u / r.length();
    this->a = -u / (E * 2);
    
    // calculate inclination
    this->i = acos(h.z / h.length());
    
    // calculate longitude of ascending node
    if (this->i == 0.0)
        this->l = 0;
    else if (n.y >= 0.0)
        this->l = acos(n.x / n.length());
    else
        this->l = 2 * M_PI - acos(n.x / n.length());
    
    // calculate argument of periapsis
    if (this->i == 0.0)
        this->w = acos(e.x / e.length());
    else if (e.z >= 0.0)
        this->w = acos(n.dot(e) / (n.length() * e.length()));
    else
        this->w = 2 * M_PI - acos(n.dot(e) / (n.length() * e.length()));
    
    // calculate true anomaly
    if (r.dot(v) >= 0.0)
        this->t = acos(e.dot(r) / (e.length() * r.length()));
    else
        this->t = 2 * M_PI - acos(e.dot(r) / (e.length() * r.length()));
    
    // calculate epoch
    this->epoch = Vector3(0,0,0);
    this->epoch = position() - r;
}

// For small eccentricities a good approximation of true anomaly can be
// obtained by the following formula (the error is of the order e^3)
double Orbit::estimateTrueAnomaly(double meanAnomaly)
{
    double M = meanAnomaly;
    return M + 2 * e * sin(M) + 1.25 * e * e * sin(2 * M);
}

double Orbit::calcEccentricAnomaly(double meanAnomaly)
{
    double t = estimateTrueAnomaly(meanAnomaly);
    double E = acos((e + cos(t)) / (1 + e * cos(t)));
    double M = E - e * sin(E);
    
    // iterate to get M closer to meanAnomaly
    double rate = 0.01;
    bool lastDec = false;
    while(1)
    {
        //printf("   using approx %f to %f\n", M, meanAnomaly);
        if (fabs(M - meanAnomaly) < 0.0000000000001)
            break;
        if (M > meanAnomaly)
        {
            E -= rate;
            lastDec = true;
        }
        else
        {
            E += rate;
            if (lastDec)
                rate *= 0.1;
        }
        M = E - e * sin(E);
    }
    
    if (meanAnomaly > M_PI && E < M_PI)
        E = 2*M_PI - E;
    
    return E;
}

void Orbit::calcTrueAnomaly(double eccentricAnomaly)
{
    double E = eccentricAnomaly;
    this->t = acos((cos(E) - e) / (1 - e * cos(E)));
    //this->t = 2 * atan2(sqrt(1+e)*sin(E/2), sqrt(1-e)*cos(E/2));
    if (eccentricAnomaly > M_PI && this->t < M_PI)
        this->t = 2*M_PI - this->t;
}

void Orbit::step(double time)
{
    double M = meanAnomaly();
    double meanM = meanMotion();
    M += meanM * time;
    while (M < -2*M_PI)
        M = M + 2*M_PI;
    if (M < 0)
        M = 2*M_PI + M;
    while (M > 2*M_PI)
        M = M - 2*M_PI;
    double E = calcEccentricAnomaly(M);
    calcTrueAnomaly(E);
    //printf("since M: %f, E=%f, t=%f\n", M, E, t);
}
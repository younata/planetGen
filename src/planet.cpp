#include "noise.h"
#include "planet.h"
#include "Orbit.h"
#include "universe.h"

planet::planet(double m, Orbit *orb, double r, double dl, double o, long ts) : CelestialBody(orb, m, r, dl, ts)
{
    moons = new std::list<planet *>();
}

void planet::tick(double dt, double seconds)
{
    if (orbit == NULL)
        return;
    orbit->step(dt);
    double scaleFactor = getScale();
    position = orbit->position() / scaleFactor;
    velocity = orbit->velocity();
    
    double PartOfCircle = dt / dayLength;
    object->rotateY(seconds * M_PI_2);
    
    for (std::list<planet*>::iterator i = moons->begin(); i != moons->end(); i++) {
        planet *p = *i;
        p->tick(seconds * (Universe::scale(defaultScale())), seconds);
    }
}

unitScale planet::defaultScale()
{
    return lightMillisecond;
}

unsigned long planet::count()
{
    unsigned long ret = 1;
    for (std::list<planet *>::iterator i = moons->begin(); i != moons->end(); i++) {
        planet *p = *i;
        ret += p->count();
    }
    return ret;
}

bool planet::generateMoon(int i, double C, double t_zero, double hs)
{
    // t_zero is period of orbit of first moon, relative to earth.
    // calculate the orbit period first, then everything else...
    double t;
    if (i == 0)
        t = t_zero;
    else
        t = t_zero * pow(C, i);

    double mMax = 0.67 * mass;
    double mMin = 0.01 * mass;

    double m = randBetween(mMin, mMax);

    double r = pow(m * pow(t, 2), 1.0/3.0);

    if (r > hs)
        return false;

    double rad = 0;

    static double earthDay = 86400.0;
    double p = m / pow(rad, 3);

    double d = 0.0; // FIXME

    double K = 1.0;
    double ob = (K * 0.00346) / (pow((d / earthDay), 2) * p); // FIXME: Oblateness
    double e = randBetween(0.0, 0.3); // eccentricity
    double in = randBetween(-10.0, 10.0); // inclination
    double l; // longitude of ascending node, 0 if inclination is 0
    if (in == 0)
        l = 0;
    else
        l = randBetween(0.0, 30.0);
    double w; // don't remember, has to do with where the orbit period starts at.
    if (e == 0)
        w = 0;
    else
        w = randBetween(-90.0, 90.0);
    double to = 0;
    Orbit *orb = new Orbit(this, r, e, in, l, w, to);

    long terrainSeed = random();
    planet *pl = new planet(m, orb, rad, d, 0, terrainSeed);

    moons->push_back(pl);
    return true;
}

void planet::generateMoons(int amount, long seed)
{
    srandom(seed);

    double C = randBetween(1.5, 2.3);
    double t = randBetween(0.4, 0.5);

    double hs = hillSphere();

    for (int i = 0; i < amount; i++)
        if (!generateMoon(i, C, t, hs))
            break;
}

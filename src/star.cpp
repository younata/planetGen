#include "star.h"
#include "planet.h"

#include <stdlib.h>
#include <math.h>

#include "orbit.h"
#include "universe.h"

star::star(double m, double a) : currentAge(a), CelestialBody(m)
{
    radius = 1.13 * pow(mass, 0.6337);
    planets = new std::list<planet *>();
    
    habitableZoneInner = sqrt(luminosity()/0.53);
    habitableZoneOuter = sqrt(luminosity()/1.1);
}

double star::luminosity()
{
    return pow(mass, 3.5);
}

double star::apparentBrightness(double distance)
{
    return luminosity() / pow(distance, 2);
}

double star::surfaceTemp()
{
    return pow(mass, 0.505);
}

double star::lifetime()
{
    return pow(mass, -2.5);
}

double star::remainingLifetime()
{
    return lifetime() - currentAge;
}

void star::tick(double dt, double seconds)
{
    if (orbit != NULL) {
        orbit->step(dt);
        double scaleFactor = getScale();
        position = orbit->position() / scaleFactor;
        velocity = orbit->velocity();
    }
    
    double PartOfCircle = dt / dayLength;
    object->rotateY(seconds * M_PI_2);
    
    for (std::list<planet *>::iterator i = planets->begin(); i != planets->end(); i++) {
        planet *p = *i;
        p->tick(seconds * (Universe::scale(defaultScale())), seconds);
    }
}

void star::generatePlanet(int i, double auConversion)
{
    double a = (0.4 + (0.3 * pow(2, i-2))) * auConversion;
    // a is in meters.
    // a needs to be in defaultScale()
    double scaleConversion = Universe::scale(defaultScale());
    double sma = a * scaleConversion;

    double m = 0;
    double r = 0;
    double d = 0; // FIXME

    double K = 0.73;
    if (i <= 4) {
        static double massEarth = 5.972e24;
        static double massMercury = 328.5e21;
        static double radiusEarth = 6378.1;
        K = 1.0;
        m = randBetween(massMercury, 2*massEarth);
        if (i == 2) { // earth..
            m = randBetween(0.1 * massEarth, 1.2 * massEarth);
        }
        r = randBetween(0.6 * radiusEarth, 1.6 * radiusEarth);
    } else {
        static double massJupiter = 1.89813e27;
        static double radiusJupiter = 71492.0;
        m = randBetween(0.001 * massJupiter, 20*massJupiter);
        r = randBetween(0.35 * radiusJupiter, 2.0 * radiusJupiter);
    }

    //double a = 0.0; // FIXME

    static double earthDay = 86400.0;

    double p = m / pow(r, 3);

    double o = (K * 0.00346) / (pow((d / earthDay), 2) * p);

    double e = randBetween(0.0, 0.3);
    double inc = randBetween(-10.0, 10.0);
    double l;
    if (inc == 0)
        l = 0;
    else
        l = randBetween(0.0, 30.0);
    double w;
    if (e == 0)
        w = 0;
    else
        w = randBetween(-90.0, 90.0);
    double t = 0;
    Orbit *orb = new Orbit(this, sma, e, inc, l, w, t);

    long terrainSeed = random();

    planet *pl = new planet(m, orb, r, d, o, terrainSeed);
    
    pl->cameraNode = this->cameraNode;
        
    // getting the planetary class.
    planetaryClass pc;
    
    pl->atmosphere = true;
    
    if (m < 2.0e23) {
        pc = ClassD;
    } else if (m > 1.0e25) {
        pc = ClassJ; // or ClassI, ClassS, ClassT
    } else {
        double b = a / auConversion;
        if (b < habitableZoneInner) {
            int foo = random()%3;
            switch (foo) {
                case 0:
                    pc = ClassH;
                    break;
                case 1:
                    pc = ClassK;
                    break;
                case 2:
                    pc = ClassY;
                    break;
                default:
                    pc = ClassD;
            }
        } else if (b > habitableZoneOuter) {
            int foo = random() % 4;
            switch (foo) {
                case 0:
                    pc = ClassH;
                    break;
                case 1:
                    pc = ClassK;
                    break;
                case 2:
                    pc = ClassP;
                    break;
                case 3:
                    pc = ClassY;
                    break;
                default:
                    pc = ClassH;
            }
        } else {
            // H,K,M,N,O,P,Y
            int foo = random() % 7;
            switch (foo) {
                case 0:
                    pc = ClassH;
                    break;
                case 1:
                    pc = ClassK;
                    break;
                case 2:
                    pc = ClassM;
                    break;
                case 3:
                    pc = ClassN;
                    break;
                case 4:
                    pc = ClassO;
                    break;
                case 5:
                    pc = ClassP;
                    break;
                case 6:
                    pc = ClassY;
                    break;
                default:
                    pc = ClassM;
            }
        }
    }
    switch (pc) {
        case ClassD:
            pl->atmosphere = false;
            pl->ocean = false;
            pl->iceCaps = 0.0;
            break;
        case ClassH:
            pl->ocean = false;
            pl->iceCaps = 0.9;
            break;
        case ClassI:
            pl->ocean = false;
            pl->iceCaps = 0.0;
            break;
        case ClassJ:
            pl->ocean = false;
            pl->iceCaps = 0.0;
            break;
        case ClassK:
            pl->atmosphere = false;
            pl->ocean = true;
            pl->iceCaps = randBetween(0.7, 0.9);
            break;
        case ClassL:
            pl->ocean = true;
            pl->iceCaps = randBetween(0.6, 0.9);
            break;
        case ClassM:
            pl->ocean = true;
            pl->iceCaps = randBetween(0.8, 0.9);
            break;
        case ClassN:
            pl->ocean = true;
            pl->iceCaps = randBetween(0.8, 0.9);
            break;
        case ClassO:
            pl->ocean = true;
            pl->iceCaps = randBetween(0.8, 0.9);
            pl->heightFactor = 0.000001;
            break;
        case ClassP:
            pl->ocean = true;
            pl->iceCaps = randBetween(0.2, 0.5);
            pl->heightFactor = 0.000001;
            break;
        case ClassS:
            pl->ocean = false;
            pl->iceCaps = 0.0;
            break;
        case ClassT:
            pl->ocean = false;
            pl->iceCaps = 0.0;
            break;
        case ClassY:
            pl->iceCaps = 0.0;
            pl->ocean = true;
            break;
    }
    
    // now, based on the class, select a color scheme...
    std::map<planetaryClass, std::list<colorList*> > colors = universe->planetaryColors;
    std::list<colorList*> c;
    for (planetaryClass i = ClassD; i <= ClassY; i++) {
        c = colors[pc];
        if (c.size() == 0) {
            if (pc == ClassY)
                pc = ClassD;
            else
                pc++;
        } else break;
    }
    int n = (int)floor(randBetween(0, c.size()));
    for (std::list<colorList*>::iterator i = c.begin(); i != c.end(); i++) {
        if (n == 0) {
            pl->setColors(**i);
            break;
        }
        n--;
    }
    
    if (scene != NULL) {
        pl->object = scene->addNode();
        pl->object->setTranslation(0, 0, 0);
    }
    
    pl->genSphere();
    
    planets->push_back(pl);
}

unitScale star::defaultScale()
{
    return lightSecond;
}

unsigned long star::countPlanets()
{
    unsigned long ret = 1;
    for (std::list<planet *>::iterator i = planets->begin(); i != planets->end(); i++) {
        planet *p = *i;
        ret += p->count();
    }
    return ret;
}

void star::generatePlanets(int amount, long seed)
{
    srandom(seed);

    double a = pow(mass * 9.945193e+14, 1.0/3.0);

    for (int i = 0; i < amount; i++)
        generatePlanet(i, a);
}

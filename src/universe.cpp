#include "universe.h"

#include <iostream>
#include <fstream>
#include <string>

Universe::Universe(unsigned long amountStars, unsigned int seed, OrbitingCamera *camera, Scene *scene) :
seed(seed),
camera(camera),
scene(scene)
{
    currentScale = lightSecond; // sun should be relatively small...
    srandom(seed);
    readClasses();
    stars = new std::list<star *>();
    for (unsigned long i = 0; i < amountStars; i++) {
        generateStar();
    }
    star *s = stars->front();
    changeFocus(s->planets->front());
}

std::list<colorList*> readColorFile(FILE *f);

std::list<colorList*> readColorFile(FILE *f)
{
    std::list<colorList*>ret;
    colorList *colors = new colorList();
    char *line = (char *)malloc(512);
    while (fgets(line, 512, f) != NULL) {
        if (strlen(line) == 0 || line[0] == '\n') {
            if (colors->size() == 0)
                continue;
            ret.push_back(colors);
            colors = new colorList();
        } else {
            float r, g, b;
            sscanf(line, (const char *)"%f %f %f", &r, &g, &b);
            Color3 c = Color3(r,g,b);
            colors->push_back(c);
        }
    }
    if (ret.back() != colors && colors->size() != 0)
        ret.push_back(colors);
    if (colors->size() == 0)
        delete colors;
    free(line);
    return ret;
}

void Universe::readClasses()
{
    FILE *f = NULL;
    
    std::string planetFiles[] = {"classd.planet", "classh.planet", "classi.planet", "classj.planet", "classk.planet",
                 "classl.planet", "classm.planet", "classn.planet", "classo.planet", "classp.planet",
                 "classs.planet", "classt.planet", "classy.planet"};
    
    std::string starFiles[] = {"classo.star", "classb.star", "classa.star", "classf.star", "classg.star", "classk.star", "classm.star"};
        
    for (planetaryClass i = ClassD; i <= ClassY; i++) {
        std::string s = std::string(FileSystem::getResourcePath()) + "/" + planetFiles[i];
        if ((f = fopen(s.c_str(), "r"))) {
            std::list<colorList*>col = readColorFile(f);
            planetaryColors.insert(std::pair<planetaryClass,std::list<colorList*> >(i, col));
            fclose(f);
            f = NULL;
        }
    }
    for (starClass i = OClass; i <= MClass; i++) {
        std::string s = std::string(FileSystem::getResourcePath()) + "/" + starFiles[i];

        if ((f = fopen(s.c_str(), "r"))) {
            std::list<colorList*>col = readColorFile(f);
            starColors.insert(std::pair<starClass,std::list<colorList*> >(i, col));
            fclose(f);
            f = NULL;
        }
    }
}

void Universe::generateStar()
{
    double r = uniformRand();
    double sm = 2.4 * pow(MATH_E, -5.6*r); // in solar masses.
    double x = sm * 1.989e30;
    
    star *s = new star(x, 0); // TODO: CHANGE THE SECOND ARGUMENT.
    s->cameraNode = camera->getCamera()->getCamera()->getNode();
    s->object = scene->addNode();
    s->object->setTranslation(0, 0, 0);
    s->scene = scene;
    int amountPlanets = (int)ceil(randBetween(0.01, 10));
    s->universe = this;
    
    // assigning colors.
    starClass sc;
    
    if (sm >= 16.0)
        sc = OClass;
    else if (sm > 2.1)
        sc = BClass;
    else if (sm > 1.4)
        sc = AClass;
    else if (sm > 1.04)
        sc = FClass;
    else if (sm > 0.8)
        sc = GClass;
    else if (sm > 0.45)
        sc = KClass;
    else // less than 0.45
        sc = MClass;
    
    std::list<colorList*> c;
    for (starClass i = OClass; i <= MClass; i++) {
        c = starColors[sc];
        if (c.size() == 0) {
            if (sc == MClass)
                sc = OClass;
            else
                sc++;
        } else break;
    }
    int n = (int)floor(randBetween(0, c.size()));
    for (std::list<colorList*>::iterator i = c.begin(); i != c.end(); i++) {
        if (n == 0) {
            s->setColors(**i);
            break;
        }
        n--;
    }
    
    s->genSphere();
    s->generatePlanets(amountPlanets, random());
    stars->push_back(s);
}

unsigned long Universe::totalBodies()
{
    unsigned long ret = 0;
    for (starList::iterator i = stars->begin(); i != stars->end(); i++) {
        star *s = *i;
        ret += s->countPlanets();
    }
    return ret;
}

double Universe::scale(unitScale currentScale)
{
    double scaleFactor = 1.0;
    switch (currentScale) {
        case meters:
            break;
        case lightMicrosecond:
            scaleFactor = 1.0/299.792458;
            break;
        case lightMillisecond:
            scaleFactor = 1.0/299792.458;
            break;
        case lightSecond:
            scaleFactor = (1.0/METERS_TO_LIGHT_SECOND);
            break;
        case lightHour:
            scaleFactor = (1.0/(METERS_TO_LIGHT_SECOND * SECONDS_TO_HOUR));
            break;
        case lightYear:
            scaleFactor = (1.0/(METERS_TO_LIGHT_SECOND * SECONDS_TO_HOUR * HOURS_TO_YEAR));
            break;
        case astronomicalUnits:
            scaleFactor = 1.0/METERS_TO_AU;
            break;
        case parsec:
            scaleFactor = 1.0/(AU_TO_PARSEC * METERS_TO_AU);
            break;
        default:
            break;
    }
    return scaleFactor;
}

void Universe::changeFocus(CelestialBody *newFocus)
{
    if (newFocus == NULL)
        return;
    camera->setTarget(newFocus->object);
    currentFocus = newFocus;
    currentScale = newFocus->defaultScale();
    newFocus->setScale(scale(currentScale));
    camera->orbit = currentFocus->radius * scale(currentScale) * 4;
    camera->orbitMin = currentFocus->radius * scale(currentScale);
    camera->scaleFactor = 0.01/scale(currentScale);
    camera->update();
}

bool Universe::render(CelestialBody *cb)
{
    Model* model = cb->object->getModel();
    if (model)
    {
        model->draw();
    }
    return true;
}

void Universe::tick(double dt)
{
    for (starList::iterator i = stars->begin(); i != stars->end(); i++) {
        star *s = *i;
        s->tick(dt * scale(lightYear), dt);
    }
    //camera->setTarget(currentFocus->object);
    camera->update();
    draw();
}

bool Universe::draw(CelestialBody *cb, double scaleFactor, Vector3 cp)
{
    Vector3 pos = cb->getPosition() * scaleFactor;
    double d = cp.distanceSquared(pos);
    cb->updateMesh(camera->getCamera()->getPosition(), camera->getCamera()->getLookAt());
    if (d <= MAX_RENDER_DISTANCE*MAX_RENDER_DISTANCE)
        render(cb);
    else {
        double s = (MAX_RENDER_DISTANCE*MAX_RENDER_DISTANCE) / d;
        cb->setScale(s);
    }
    if (cb->defaultScale() == lightSecond) {
        star *s = (star *)cb;
        for (planetList::iterator i = s->planets->begin(); i != s->planets->end(); i++) {
            if (!draw(*i, scaleFactor, cp))
                break;
        }
    } else if (cb->defaultScale() == lightMillisecond) {
        planet *s = (planet *)cb;
        for (planetList::iterator i = s->moons->begin(); i != s->moons->end(); i++) {
            if (!draw(*i, scaleFactor, cp))
                break;
        }
    }
    return true;
}

void Universe::draw()
{
    double scaleFactor = scale(currentScale);
    Vector3 cp = camera->getCamera()->getPosition();
    // get the positions of everything.
    for (starList::iterator i = stars->begin(); i != stars->end(); i++) {
        star *s = *i;
        bool didRender = false;
        if (s == currentFocus)
            didRender = render(s);
        else {
            draw(s, scaleFactor, cp);
        }
    }
}

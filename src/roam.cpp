#include "roam.h"

#include <limits>
#include "CelestialBody.h"

bool frustrumContains(Frustum frustrum, Vector3 p)
{
    Vector3 f = frustrum.getFar().getNormal();
    Vector3 n = frustrum.getNear().getNormal();
    Vector3 l = frustrum.getLeft().getNormal();
    Vector3 r = frustrum.getRight().getNormal();
    Vector3 t = frustrum.getTop().getNormal();
    Vector3 b = frustrum.getBottom().getNormal();
    
    if (f.dot(p) < 0 && n.dot(p) > 0)
        return false;
    if (l.dot(p) < 0 && r.dot(p) > 0)
        return false;
    if (t.dot(p) < 0 && b.dot(p) > 0)
        return false;
    
    return true;
}

bool angle3Points(Vector3 a, Vector3 b, Vector3 c)
{
    float C = a.distanceSquared(b);
    float B = c.distanceSquared(a);
    float A = b.distanceSquared(c);
    // law of cosines: C^2 = B^2 + A^2 - 2bc * cos(angle)
    return C <= (A+B);
}

float sqrt_approx(float z)
{
    union { int tmp; float f; } u;
    u.f = z;
    u.tmp -= 1 << 23; /* Subtract 2^m. */
    u.tmp >>= 1; /* Divide by 2. */
    u.tmp += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
    return u.f;
}

struct splitSorter {
    Vector3 cp;
    Vector3 center;
    bool operator() (RoamTriangle *i, RoamTriangle *j) {
        Vector3 ic = i->incenter + center;
        Vector3 jc = j->incenter + center;
        double d = (double)sqrt_approx(cp.distanceSquared(ic));
        double f = (double)sqrt_approx(cp.distanceSquared(jc));
        double vd = i->calculateVisualError(d);
        double vf = j->calculateVisualError(f);

        return vd < vf;
    }
};

// ROAM algorithm itself
RoamSphere::RoamSphere(double mve, double minve, int md, double r, double o, CelestialBody *body) : maxVisualError(mve), minVisualError(minve), maxDepth(md), radius(1.0), oblateness(o), body(body), id(0), did(0)
{
    triangles = new std::list<RoamTriangle *>();
    mergeQueue = NULL;
    splitQueue = NULL;
    RoamTriangle *t, *b;
    sterrain mp;
    float x,y,z;
    
    r = 1.0;
    
    maxTriangles = 0;
    
    float error = 128.0;
    for (int i = 0; i < 6; i++) {
        float x,y,z;
        Vector3 vf[3];
        Vector3 other;
        switch (i) {
            case 0:
                x = 0; y = 0; z = r;
                vf[0] = Vector3(-r, -r, r);
                vf[1] = Vector3(r, -r, r);
                vf[2] = Vector3(-r, r, r);
                other = Vector3(r, r, r);
                break;
            case 1: // right-face
                x = r; y = 0; z = 0;
                vf[0] = Vector3(r, -r, r);
                vf[1] = Vector3(r, -r, -r);
                vf[2] = Vector3(r, r, r);
                other = Vector3(r, r, -r);
                break;
            case 2: // back-face
                x = 0; y = 0; z = -r;
                vf[0] = Vector3(r, -r, -r);
                vf[1] = Vector3(-r, -r, -r);
                vf[2] = Vector3(r, r, -r);
                other = Vector3(-r, r, -r);
                break;
            case 3: // left-face
                x = -r; y = 0; z = 0;
                vf[0] = Vector3(-r, -r, -r);
                vf[1] = Vector3(-r, -r, r);
                vf[2] = Vector3(-r, r, -r);
                other = Vector3(-r, r, r);
                break;
            case 4: // bottom-face
                x = 0; y = -r; z = 0;
                vf[0] = Vector3(-r, -r, -r);
                vf[1] = Vector3(r, -r, -r);
                vf[2] = Vector3(-r, -r, r);
                other = Vector3(r, -r, r);
                break;
            case 5: // top-face
                x = 0; y = r; z = 0;
                vf[0] = Vector3(-r, r, r);
                vf[1] = Vector3(r, r, r);
                vf[2] = Vector3(-r, r, -r);
                other = Vector3(r, r, -r);
                break;
        }
        
        for (int i = 0; i < 3; i++) {
            vf[i].normalize();
            vf[i].scale(r);
        }
        other.normalize();
        other.scale(r);
        
        mp = sterrain(x,y,z);
        //pl->terrainAlgorithm(t->midpoint, (float)t->size);
        
        sterrain foo, bar;
        float oct = 8;
        foo = body->terrainAlgorithm(vf[1], oct);
        bar = body->terrainAlgorithm(vf[2], oct);

        t = new RoamTriangle(NULL, 0, id++);
        t->setMidpoint(mp);
        t->setPosition(body->terrainAlgorithm(vf[0], oct), foo, bar);
        t->error = error;
        triangles->push_back(t);

        b = new RoamTriangle(NULL, 0, id++);
        b->setMidpoint(mp);
        b->setPosition(body->terrainAlgorithm(other, oct), bar, foo);
        b->error = error;
        triangles->push_back(b);
    }
    RoamTriangle *fb, *ft, *rb, *rt, *ab, *at, *lb, *lt, *bb, *bt, *tb, *tt;
    assert(triangles->size() != 0);
    int j = 0;
    for (std::list<RoamTriangle *>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        assert(*i != NULL);
        switch(j) {
            case 0:
                fb = *i;
                break;
            case 1:
                ft = *i;
                break;
            case 2:
                rb = *i;
                break;
            case 3:
                rt = *i;
                break;
            case 4:
                ab = *i;
                break;
            case 5:
                at = *i;
                break;
            case 6:
                lb = *i;
                break;
            case 7:
                lt = *i;
                break;
            case 8:
                bb = *i;
                break;
            case 9:
                bt = *i;
                break;
            case 10:
                tb = *i;
                break;
            case 11:
                tt = *i;
                break;
        }
        j++;
    }

    fb->setEdges(ft, lt, bt); // 0
    ft->setEdges(fb, rb, tb); // 1

    rb->setEdges(rt, ft, bt); // 2
    rt->setEdges(rb, ab, tt); // 3

    ab->setEdges(at, rt, bb); // 4
    at->setEdges(ab, lb, tt); // 5

    lb->setEdges(lt, at, bb); // 6
    lt->setEdges(lb, fb, tb); // 7

    bb->setEdges(bt, lb, ab); // 8
    bt->setEdges(bb, rb, fb); // 9

    tb->setEdges(tt, lt, ft); // 10
    tt->setEdges(tb, rt, at); // 11
    
#ifdef DEBUG
    for (std::list<RoamTriangle *>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        RoamTriangle *t = *i;
        assert(t->confirmVertex(t->edge0, 0));
        assert(t->confirmVertex(t->edge1, 1));
        assert(t->confirmVertex(t->edge2, 2));
    }
#endif
}

void RoamSphere::setPlanet(CelestialBody *b) { body = b; }

bool RoamSphere::isPlanetNull() { return body == NULL; }

bool RoamSphere::CollisionCheck(Vector3 camera)
{
    sterrain t = body->terrainAlgorithm(camera, (float)triangles->front()->size);
    return (camera.lengthSquared() - t.lengthSquared()) < 0.0f;
}

void RoamSphere::split(RoamTriangle *t)
{
    if (t->size == maxDepth)
        return;

    if (!t->edge0->edge0->equals(t)) {// if o is NOT the neighbor...
        split(t->edge0);
    }
    RoamTriangle *o = t->edge0;
    
#ifdef DEBUG
    assert(t->vertexes[1] == o->vertexes[2]);
    assert(t->vertexes[2] == o->vertexes[1]);
#endif
    
    RoamTriangle *n = new RoamTriangle(t, ++t->size, id++);

    RoamTriangle *p = new RoamTriangle(o, ++o->size, id++);
    
    /*
    t->error /= 2.0f;
    o->error /= 2.0f;
    n->error = t->error;
    p->error = o->error;
     */
    
    // update position
    t->midpoint = sterrain((t->vertexes[1] + t->vertexes[2])/2.0);
    sterrain a = t->midpoint;
    sterrain mp = body->terrainAlgorithm(t->midpoint, (float)t->size);
    double error = sqrt_approx(mp.distanceSquared(a));
    error *= radius;
    t->error = error;
    o->error = error;
    n->error = error;
    p->error = error;
    sterrain t_0 = sterrain(t->vertexes[0]);
    sterrain t_1 = sterrain(t->vertexes[1]);
    sterrain t_2 = sterrain(t->vertexes[2]);
    
    sterrain o_0 = sterrain(o->vertexes[0]);
    sterrain o_1 = sterrain(o->vertexes[1]);
    
    sterrain o_2 = sterrain(o->vertexes[2]);
#ifdef DEBUG
    assert(o_1 == t_2);
    assert(o_2 == t_1);
#endif
    
    t->setPosition(mp, t_2, t_0);
    n->setPosition(mp, t_0, t_1);
    
    o->setPosition(mp, o_2, o_0);
    p->setPosition(mp, o_0, o_1);

    // reset edges.
    RoamTriangle *t_e1 = t->edge1;
    RoamTriangle *t_e2 = t->edge2;
    
#ifdef DEBUG
    assert(t_e1 != NULL);
    assert(t_e2 != NULL);
#endif

    RoamTriangle *o_e1 = o->edge1;
    RoamTriangle *o_e2 = o->edge2;
    
#ifdef DEBUG
    assert(o_e1 != NULL);
    assert(o_e2 != NULL);
#endif

    t->setEdges(t_e1, n, p);
    n->setEdges(t_e2, o, t);
#ifdef DEBUG
    assert(t->confirmVertex(t_e1, 0));
    assert(t->confirmVertex(n, 1));
    assert(t->confirmVertex(p, 2));

    assert(n->confirmVertex(t_e2, 0));
    assert(n->confirmVertex(o, 1));
    assert(n->confirmVertex(t, 2));
#endif

    if (t_e2->edge0->equals(t)) {
        t_e2->edge0 = n;
    } else if (t_e2->edge1->equals(t)) {
        t_e2->edge1 = n;
    } else if (t_e2->edge2->equals(t)) {
        t_e2->edge2 = n;
    } else
        assert(false);
    
    o->setEdges(o_e1, p, n);
    p->setEdges(o_e2, t, o);
    
#ifdef DEBUG
    assert(o->confirmVertex(o_e1, 0));
    assert(o->confirmVertex(p, 1));
    assert(o->confirmVertex(n, 2));
    
    assert(p->confirmVertex(o_e2, 0));
    assert(p->confirmVertex(t, 1));
    assert(p->confirmVertex(o, 2));
#endif
    
    if (o_e2->edge0->equals(o)) {
        o_e2->edge0 = p;
    } else if (o_e2->edge1->equals(o)) {
        o_e2->edge1 = p;
    } else if (o_e2->edge2->equals(o)) {
        o_e2->edge2 = p;
    } else
        assert(false);

    if (t->isInDiamond() && t->diamond != NULL)
        removeDiamond(t->diamond);

    if (o->isInDiamond() && o->diamond != NULL)
        removeDiamond(o->diamond);
    
    RoamDiamond *d = createDiamond(t);
#ifdef DEBUG
    assert(d != NULL);
#endif

    triangles->push_back(n);
    triangles->push_back(p);
    
    splitQueue->remove(t);
    splitQueue->remove(o);
    
    Vector3 cp = cameraNode->getTranslationWorld();
    Frustum frustum = cameraNode->getCamera()->getFrustum();
    Vector3 ic = n->incenter + center;
    double err = (double)sqrt_approx(cp.distanceSquared(ic));
    double ve = t->calculateVisualError(err);
    if (frustrumContains(frustum, ic) && angle3Points(cp, ic, center) && ve > maxVisualError) {
        splitQueue->push_back(n);
    }
    
    ic = p->incenter + center;
    err = (double)sqrt_approx(cp.distanceSquared(ic));
    ve = t->calculateVisualError(err);
    if (frustrumContains(frustum, ic) && angle3Points(cp, ic, center) && ve > maxVisualError) {
        splitQueue->push_back(p);
    }
}

void RoamSphere::removeDiamond(RoamDiamond *d) { d->rm(); }

void RoamSphere::merge(RoamDiamond *d)
{
    RoamTriangle *t, *n, *o, *p;
    t = d->parent[0];
    n = d->child[0];
    
    o = d->parent[1];
    p = d->child[1];

#ifdef DEBUG
    assert(n->parent == t);
    assert(t->child == n);
    assert(t->edge1->equals(n));
    assert(n->edge2->equals(t));
    
    assert(p->parent == o);
    assert(o->child == p);
    assert(o->edge1->equals(p));
    assert(p->edge2->equals(o));
    
    assert(t->vertexes[0] == n->vertexes[0]);
    assert(n->vertexes[0] == p->vertexes[0]);
    assert(o->vertexes[0] == p->vertexes[0]);
    
    assert(t->confirmVertex(n, 1));
    assert(o->confirmVertex(p, 1));
    
    assert(t->size == n->size);
    assert(o->size == p->size);
#endif // DEBUG
    
    t->child = NULL;
    o->child = NULL;

    t->midpoint = sterrain(t->vertexes[0]);
    o->midpoint = sterrain(t->vertexes[0]);
    
    sterrain mp = sterrain((t->vertexes[1] + t->vertexes[2])/2.0);
    sterrain a = t->midpoint;
    double error = sqrt_approx(mp.distanceSquared(a));
    error *= radius;
    t->error = error;
    o->error = error;
    
    t->size--;
    o->size--;
    
    t->setEdges(o, t->edge0, n->edge0);
    t->vertexes[0] = n->vertexes[1];
    assert(t->vertexes[0] == t->vertexes[2]);
    t->vertexes[2] = t->vertexes[1];
    t->vertexes[1] = n->vertexes[2];
    
    o->setEdges(t, o->edge0, p->edge0);
    o->vertexes[0] = p->vertexes[1];
    assert(o->vertexes[0] == o->vertexes[2]);
    o->vertexes[2] = o->vertexes[1];
    o->vertexes[1] = p->vertexes[2];
    
    // update the edges...
    RoamTriangle *e0 = n->edge0;
    if (e0->edge0->equals(n)) {
        e0->edge0 = t;
    } else if (e0->edge1->equals(n)) {
        e0->edge1 = t;
    } else if (e0->edge2->equals(n)) {
        e0->edge2 = t;
    } else assert(false);
        
    e0 = p->edge0;
    if (e0->edge0->equals(p)) {
        e0->edge0 = o;
    } else if (e0->edge1->equals(p)) {
        e0->edge1 = o;
    } else if (e0->edge2->equals(p)) {
        e0->edge2 = o;
    } else assert(false);
    
    
#ifdef DEBUG
    assert(t->edge1->edge0->equals(t) || t->edge1->edge1->equals(t) || t->edge1->edge2->equals(t));
    assert(o->edge1->edge0->equals(o) || o->edge1->edge1->equals(o) || o->edge1->edge2->equals(o));
    
    assert(t->edge2->edge0->equals(t) || t->edge2->edge1->equals(t) || t->edge2->edge2->equals(t));
    assert(o->edge2->edge0->equals(o) || o->edge2->edge1->equals(o) || o->edge2->edge2->equals(o));
    
    assert(t->edge0->edge0->equals(t));
    assert(o->edge0->edge0->equals(o));
    
    assert(t->vertexes[1] == o->vertexes[2]);
    assert(o->vertexes[1] == t->vertexes[2]);
    
    assert(t->midpoint == o->midpoint);
    
    assert(t->confirmVertex(t->edge0, 0));
    assert(t->confirmVertex(t->edge1, 1));
    assert(t->confirmVertex(t->edge2, 2));
    
    assert(o->confirmVertex(o->edge0, 0));
    assert(o->confirmVertex(o->edge1, 1));
    assert(o->confirmVertex(o->edge2, 2));
    
    assert(!n->containsEdge());
    assert(!p->containsEdge());
    
    assert(n->vertexes[1] == t->vertexes[0]);
    assert(p->vertexes[1] == o->vertexes[0]);
#endif // DEBUG
    
    triangles->remove(n);
    triangles->remove(p);
    
    mergeQueue->remove(n);
    mergeQueue->remove(p);
    mergeQueue->remove(t);
    mergeQueue->remove(o);
    
    removeDiamond(d);
        
    delete d;
    delete n;
    delete p;
    d = NULL;
    n = NULL;
    p = NULL;

    if (t->isInDiamond())
        RoamDiamond *d = createDiamond(t);
    if (o->isInDiamond())
        RoamDiamond *d = createDiamond(o);
}

int RoamSphere::checkForReferences(RoamTriangle *t, bool print)
{
    int ret = 0;
    for (std::list<RoamTriangle*>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        RoamTriangle *n = *i;
        if (n->parent != NULL && n->parent->equals(t)) {
            if (print)
                printf("triangle %ld has t as parent\n", n->id);
            ret++;
        }
        if (n->child != NULL && n->child->equals(t)) {
            if (print)
                printf("triangle %ld has t as child\n", n->id);
            ret++;
        }
        if (n->edge0->equals(t)) {
            if (print)
                printf("triangle %ld has t as edge0\n", n->id);
            ret++;
        }
        if (n->edge1->equals(t)) {
            if (print)
                printf("triangle %ld has t as edge1\n", n->id);
            ret++;
        }
        if (n->edge2->equals(t)) {
            if (print)
                printf("triangle %ld has t as edge2\n", n->id);
            ret++;
        }
        if (n->diamond != NULL) {
            RoamDiamond *d = n->diamond;
            for (int j = 0; j < 2; j++) {
                if (d->parent[j]->equals(t)) {
                    if (print)
                        printf("diamond %ld has t as parent[%d]\n", d->id, j);
                    ret++;
                }
                if (d->child[j]->equals(t)) {
                    if (print)
                        printf("diamond %ld has t as child[%d]\n", d->id, j);
                    ret++;
                }
            }
        }
    }
    return ret;
}

RoamDiamond *RoamSphere::createDiamond(RoamTriangle *t)
{
    if (t->edge1->child != NULL && t->edge1->child->equals(t))
        t = t->edge1;
    if (t->edge2->child != NULL && t->edge2->child->equals(t))
        t = t->edge2;
        
    RoamTriangle *op = t->edge1->edge1;
    RoamTriangle *e1 = t->edge1;
    RoamTriangle *e2 = t->edge2;

    RoamTriangle *p1 = NULL, *p2 = NULL, *c1 = NULL, *c2 = NULL;
    p1 = t;
    p2 = op;

    if (e1->parent == NULL)
        return NULL;
    if (e1->parent->equals(op)) {
        c2 = e1;
        c1 = e2;
    } else if (e1->parent->equals(t)) {
        c2 = e2;
        c1 = e1;
    } else
        return NULL;
    
#ifdef DEBUG
    assert(c1 != NULL);
    assert(c2 != NULL);
#endif

    if (!p1->isInDiamondWith(c1, c2, p2))
        return NULL;
    RoamDiamond *d = new RoamDiamond(p1, p2, c1, c2, did++);
    p1->diamond = d;
    p2->diamond = d;
    c1->diamond = d;
    c2->diamond = d;
    return d;
}

bool RoamSphere::update(Vector3 cp, Vector3 la, Frustum frustum)
{
    // TODO: fix visualError such that it return 0 if the triangle is behind the camera, even if it is microns away from the camera.
    Vector3 ncp;
    
    bool ret = false;
    
    if (splitQueue == NULL) {
        splitQueue = new std::list<RoamTriangle *>();
    }
    if (mergeQueue == NULL) {
        mergeQueue = new std::list<RoamTriangle *>();
    }
    
    if (cameraNode != NULL) {
        for (std::list<RoamTriangle*>::iterator i = triangles->begin(); i != triangles->end(); i++) {
            bool isIn = false;
            RoamTriangle *t = *i;
            Vector3 ic = t->incenter + center;
            double d = (double)sqrt_approx(cp.distanceSquared(ic));
            double ve = t->calculateVisualError(d);
            if (frustrumContains(frustum, ic) && angle3Points(cp, ic, center) && ve > maxVisualError) {
                splitQueue->push_back(t);
            } else {
                mergeQueue->push_back(t);
            }
        }
    }
    
    splitSorter s;
    s.cp = cp;
    s.center = center;
    splitQueue->sort(s);
            
#ifdef DEBUG
    assert(confirmAllVertices());
#endif
    
    int currentSize = triangles->size();
    
    Frustum frustrum = cameraNode->getCamera()->getFrustum();
    if (maxTriangles == 0)
        maxTriangles = MAX_TRIANGLES;
    
    while (triangles->size() < maxTriangles && splitQueue->size() > 0) {
        ret = true;
        RoamTriangle *t = splitQueue->front();
        split(t);
    }
    splitQueue->clear();
    
    mergeQueue->sort(s);
    mergeQueue->reverse();
    
    while (mergeQueue->size() > 0) {
        ret = true;
        RoamTriangle *t = mergeQueue->front();
        if (t == NULL) {
            mergeQueue->pop_front();
            continue;
        }
        Vector3 ic = t->incenter + center;
        double d = (double)sqrt_approx(cp.distanceSquared(ic));
        double ve = t->calculateVisualError(d);
        if (ve > minVisualError)
            break;
        if (t->diamond != NULL && t->isInDiamond())
            merge(t->diamond);
        else
            mergeQueue->pop_front();
    }
    mergeQueue->clear();
    
    return ret;
}

bool RoamSphere::confirmAllVertices()
{
    for (std::list<RoamTriangle *>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        RoamTriangle *t = *i;
        if (!t->isValidTriangle())
            return false;
        if (!t->confirmVertex(t->edge0, 0))
            return false;
        if (!t->confirmVertex(t->edge1, 1))
            return false;
        if (!t->confirmVertex(t->edge2, 2))
            return false;
    }
    return true;
}

std::list<sterrain> *RoamSphere::getVertices()
{
    std::list<sterrain> *ret = new std::list<sterrain>();
    for (std::list<RoamTriangle*>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        RoamTriangle *t = *i;
        for (int j = 0; j < 3; j++)
            ret->push_back(t->vertexes[j]);
    }
    assert(ret->size() == triangles->size()*3);
    return ret;
}

void RoamSphere::rm()
{
    for (std::list<RoamTriangle*>::iterator i = triangles->begin(); i != triangles->end(); i++) {
        RoamTriangle *t = *i;
        if (t->diamond != NULL)
            removeDiamond(t->diamond);
        delete t;
    }
    splitQueue->clear();
    mergeQueue->clear();
    triangles->clear();
    delete splitQueue;
    delete mergeQueue;
    delete triangles;
    // now it is safe to remove this.
}

// RoamTriangle stuff
RoamTriangle::RoamTriangle(RoamTriangle *p, int s, unsigned long i)
{
    diamond = NULL;
    parent = p;
    if (p != NULL)
        p->child = this;
    child = NULL;
    edge0 = NULL;
    edge1 = NULL;
    edge2 = NULL;
    size = s;
    error = 0;
    id = i;
}

inline bool RoamTriangle::operator==(const RoamTriangle& t) const
{
    return (vertexes[0] == t.vertexes[0]) && (vertexes[1] == t.vertexes[1]) && (vertexes[2] == t.vertexes[2]);
}

void RoamTriangle::setEdges(RoamTriangle *e0, RoamTriangle *e1, RoamTriangle *e2)
{
    edge0 = e0;
    edge1 = e1;
    edge2 = e2;
}

void RoamTriangle::setPosition(sterrain e0, sterrain e1, sterrain e2)
{
    vertexes[0] = sterrain(e0);
    vertexes[1] = sterrain(e1);
    vertexes[2] = sterrain(e2);

    float x,y,z;
    x = e1.distance(e2); // distance of side 0
    y = e2.distance(e0); // distance of side 1
    z = e0.distance(e1); // distance of side 2

    float p = x + y + z;

    x *= (e0.x + e1.x + e2.x);
    y *= (e0.y + e1.y + e2.y);
    z *= (e0.z + e1.z + e2.z);
    
    midpoint = sterrain((e1 + e2)/2.0);

    incenter = Vector3(x/p, y/p, z/p);
}

bool RoamTriangle::confirmVertex(RoamTriangle *t, int edge)
{
    if (edge == 0) {
        return (t->vertexes[0] == vertexes[1] || t->vertexes[1] == vertexes[1] || t->vertexes[2] == vertexes[1]) &&
               (t->vertexes[0] == vertexes[2] || t->vertexes[1] == vertexes[2] || t->vertexes[2] == vertexes[2]);
    } else if (edge == 1) {
        return (t->vertexes[0] == vertexes[0] || t->vertexes[1] == vertexes[0] || t->vertexes[2] == vertexes[0]) &&
               (t->vertexes[0] == vertexes[2] || t->vertexes[1] == vertexes[2] || t->vertexes[2] == vertexes[2]);
    } else if (edge == 2) {
        bool zero, one, two;
        zero = t->vertexes[0] == vertexes[1];
        one = t->vertexes[1] == vertexes[1];
        two = t->vertexes[2] == vertexes[1];
        bool zed, une, dos;
        zed = t->vertexes[0] == vertexes[0];
        une = t->vertexes[1] == vertexes[0];
        dos = t->vertexes[2] == vertexes[0];
        
        return (zero || one || two) && (zed || une || dos);
    } else
        assert(edge == 0 || edge == 1 || edge == 2);
    return false;
}

bool RoamTriangle::isValidTriangle() { return (edge0 != NULL && edge1 != NULL && edge2 != NULL); }

bool RoamTriangle::equals(RoamTriangle *t)
{
    return (vertexes[0] == t->vertexes[0]) && (vertexes[1] == t->vertexes[1]) && (vertexes[2] == t->vertexes[2]);
}

float RoamTriangle::getPriority(Vector3 pos, Vector3 heading, float horizon)
{
    return 0;
}

void RoamTriangle::setMidpoint(sterrain m) { midpoint = sterrain(m); }

double RoamTriangle::calculateVisualError(double distanceToCamera) { return error / distanceToCamera; }

bool RoamTriangle::containsEdge()
{
    if (edge0->edge0->equals(this))
        return true;
    if (edge0->edge1->equals(this))
        return true;
    if (edge0->edge2->equals(this))
        return true;
    
    if (edge1->edge0->equals(this))
        return true;
    if (edge1->edge1->equals(this))
        return true;
    if (edge1->edge2->equals(this))
        return true;
    
    if (edge2->edge0->equals(this))
        return true;
    if (edge2->edge1->equals(this))
        return true;
    if (edge2->edge2->equals(this))
        return true;
    
    return false;
}

bool RoamTriangle::isInDiamond()
{
    RoamTriangle *op = edge2->edge2;
    if (!edge1->edge1->equals(op))
        return false;

    if (!(op->equals(edge1->edge1) && edge1->edge2->equals(this) && edge2->edge1->equals(this)))
        return false;
    
    if ((child != NULL && op->child != NULL) &&
        ((child->equals(edge2) && op->child->equals(edge1)) || (child->equals(edge1) && op->child->equals(edge2))))
        return true;
    
    if (((parent != NULL && (parent->equals(edge1) || parent->equals(edge2))) &&
         op->parent != NULL && (op->parent->equals(edge1) || op->parent->equals(edge2))))
        return true;
    return false;
}

bool RoamTriangle::isInDiamondWith(RoamTriangle *e1, RoamTriangle *e2, RoamTriangle *op)
{
    if (!isInDiamond())
        return false;
    if (!(e1->equals(edge1) || e1->equals(edge2)))
        return false;
    if (!(e2->equals(edge1) || e2->equals(edge2)))
        return false;
    if (!op->equals(e2->edge2) && !op->equals(e1->edge1))
        return false;

    return true;
}

// RoamDiamond stuff
RoamDiamond::RoamDiamond(RoamTriangle *p1 ,RoamTriangle *p2, RoamTriangle *c1, RoamTriangle *c2, unsigned long i) : id(i)
{
#ifdef DEBUG
    assert(p1->isInDiamondWith(c1, c2, p2));
#endif
    parent[0] = p1;
    parent[1] = p2;
    child[0] = c1;
    child[1] = c2;

#ifdef DEBUG
    assert(p1->vertexes[0] == p2->vertexes[0]);
    assert(p1->vertexes[0] == c1->vertexes[0]);
    assert(p2->vertexes[0] == c2->vertexes[0]);
#endif
    // vertexes of all of this SHOULD be the same.
    midpoint = sterrain(p1->vertexes[0]);
    offset = (p1->error + p2->error + c1->error + c2->error) / 4.0f;
}

inline bool RoamDiamond::operator==(const RoamDiamond& d) const
{
    return parent[0]->equals(d.parent[0]) && parent[1]->equals(d.parent[1]) && child[0]->equals(d.child[0]) && child[1]->equals(d.child[1]);
}

void RoamDiamond::rm()
{
    for (int i = 0; i < 2; i++) {
        parent[i]->diamond = NULL;
        child[i]->diamond = NULL;
    }
}

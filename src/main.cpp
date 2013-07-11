#include "main.h"

#include <limits>

// Declare our game instance
main game;

main::main()
    : _scene(NULL)
{
}

void main::initialize()
{
    displayScreen(this, &main::drawSplash, NULL, 1000L);
    
    setMultiTouch(true);
    
    _scene = Scene::create();
    Node *cameraNode = _scene->addNode("Camera");
    Camera *camera = Camera::createPerspective(45.0f, (float)getWidth() / (float)getHeight(), 0.25f, METERS_TO_AU);//std::numeric_limits<float>::max());
    cameraNode->setCamera(camera);
    cameraNode->setTranslation(0.0f, 0.0f, 40.0f);
    _scene->setActiveCamera(camera);
    SAFE_RELEASE(camera);

    _scene->setAmbientColor(1.0, 1.0, 1.0);
    _scene->setLightColor(1.0, 1.0, 1.0);
    
    _font = Font::create("arial.gpb");
    
    ///*
    Light *l = Light::createPoint(Vector3(1.0,1.0,1.0), std::numeric_limits<float>::max());
    Node *ln = _scene->addNode("lightNode");
    ln->setTranslation(Vector3(0,0,0));
    ln->setLight(l);
    
    ///*
    w = new DemoWorld();
    double ms = 1.989e30;
    //ms = 1.329e20;
    w->sun = new star(ms, 0);
    float earthRadius = 6378100.0;
    earthRadius = 10.0;
    Orbit *earthOrbit = new Orbit(w->sun, METERS_TO_AU, 0, 0, 0, 0, 0);
    //w->earth = new planet(5.972e24, NULL, 6378100, 24*60*60, 0, 1337);
    long earthSeed = 1337;
    earthSeed = 2048;
    w->earth = new planet(5.972e24, earthOrbit, earthRadius, 24*60*60, 0, earthSeed);
    assert(w->earth->orbit != NULL);
    std::vector<Color3> c;
    c.push_back(Color3(.5, .39, .2));
    c.push_back(Color3(.5, .39, .2));
    c.push_back(Color3(.5, .39, .2));
    c.push_back(Color3(.2, .3, 0));
    c.push_back(Color3(.085, .2, .04));
    c.push_back(Color3(.065, .22, .04));
    c.push_back(Color3(.5, .42, .28));
    c.push_back(Color3(.6, .5, .23));
    c.push_back(Color3(1, 1, 1));
    c.push_back(Color3(1, 1, 1));
    c.push_back(Color3(0.12, 0.3, 0.6));

    w->earth->setColors(c);
    w->earth->cameraNode = _scene->getActiveCamera()->getNode();
    w->earth->setTriangles(50000);
    w->earth->genSphere();
    w->earth->ocean = true;
    w->earth->atmosphere = true;
        
    Node *earthNode = _scene->addNode("earthNode");
    w->earth->object = earthNode;
    // */
    
    simpleCamera *sc = new simpleCamera();
    sc->setCamera(_scene->getActiveCamera());
    oc = new OrbitingCamera();
    oc->reset();
    oc->orbitMin = 10.0;
    oc->orbit = 100.0;
    oc->setDisplayStuff(getWidth(), getHeight());
    oc->setCamera(sc);
    universe = NULL;
        
    //universe = new Universe(1, 69, oc, _scene);
}

void main::finalize()
{
    SAFE_RELEASE(_scene);
}

void main::update(float elapsedTime)
{
    // Rotate model
    //_scene->findNode("box")->rotateY(MATH_DEG_TO_RAD((float)elapsedTime / 1000.0f * 180.0f));
    ///*
    if (w != NULL) {
        w->earth->tick(elapsedTime, meters);
        //w->earth->object->setTranslation(w->earth->getPosition());
        w->earth->updateMesh(oc->getCamera()->getPosition(), w->earth->getPosition());
        
        if (oc != NULL) {
            oc->setTarget(w->earth->object->getTranslationWorld());
            oc->update();
        }
    }
    // */
    ///*
    if (universe != NULL)
        universe->tick(elapsedTime);
    // */
}

void main::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Visit all the nodes in the scene for drawing
    _scene->visit(this, &main::drawScene);
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 10, 10, getFrameRate());
}

bool main::drawScene(Node* node)
{
    ///*
    // If the node visited contains a model, draw it
    Model* model = node->getModel(); 
    if (model)
    {
        model->draw();
    }
    // */
    return true;
}

void main::drawSplash(void* param)
{
    clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);
    SpriteBatch* batch = SpriteBatch::create("res/logo_powered_white.png");
    batch->start();
    batch->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
    batch->finish();
    SAFE_DELETE(batch);
}
void main::drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps)
{
    char buffer[1024];
    Vector3 cp = oc->getCamera()->getPosition();
    if (w != NULL)
        sprintf(buffer, "FPS: %u\nPosition: (%0.2f, %0.2f %0.2f)\nTriangles: %d", fps, cp.x, cp.y, cp.z, w->earth->amountOfTriangles());
    else if (universe != NULL)
        sprintf(buffer, "FPS: %u\nPosition: (%0.2f, %0.2f %0.2f)\nTotal Bodies: %ld", fps, cp.x, cp.y, cp.z, universe->totalBodies());
    font->start();
    font->drawText(buffer, x, y, color, 28);
    font->finish();
}

void main::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
        }
    }
}

void main::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (oc != NULL)
        oc->touchEvent(evt, x, y, contactIndex);
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

bool main::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    if (oc != NULL)
        oc->mouseEvent(evt, x, y, wheelDelta);
    switch (evt)
    {
        case Mouse::MOUSE_MOVE:
            break;
        case Mouse::MOUSE_PRESS_LEFT_BUTTON:
            break;
        case Mouse::MOUSE_PRESS_MIDDLE_BUTTON:
            break;
        case Mouse::MOUSE_PRESS_RIGHT_BUTTON:
            break;
        case Mouse::MOUSE_RELEASE_LEFT_BUTTON:
            break;
        case Mouse::MOUSE_RELEASE_MIDDLE_BUTTON:
            break;
        case Mouse::MOUSE_RELEASE_RIGHT_BUTTON:
            break;
        case Mouse::MOUSE_WHEEL:
            break;
    }
    return false;
}

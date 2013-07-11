//
//  inputEvent.cpp
//  SpaceSim
//
//  Created by Rachel Brindle on 1/15/13.
//
//

#include "inputEvent.h"

/*
inputEvent::inputEvent(Keyboard::KeyEvent evt, int key) : event.k(evt), key(key), type(EVENT_KEY), {}

inputEvent::inputEvent(Touch::TouchEvent evt, int x, int y, unsigned int hash) : event.t(evt), hash(hash), type(EVENT_TOUCH)
{
    location = new Vector2(x,y);
}

inputEvent::inputEvent(Mouse::MouseEvent evt, int x, int y) : event.m(evt), type(EVENT_MOUSE)
{
    location = new Vector2(x,y);
}

inline bool inputEvent::operator==(const inputEvent& e) const
{
    switch(type) {
        case EVENT_TOUCH:
            return this->hash == e.identifier();
    }
}*/
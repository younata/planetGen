//
//  inputEvent.h
//  SpaceSim
//
//  Created by Rachel Brindle on 1/15/13.
//
//

#ifndef __SpaceSim__inputEvent__
#define __SpaceSim__inputEvent__

#include <iostream>
#include "gameplay.h"

enum E_Input_Event {
    EVENT_TOUCH,
    EVENT_KEY,
    EVENT_MOUSE,
};

using namespace gameplay;

class inputEvent {
    Vector2 *location;
    Vector2 *previousLocation;
    
    int key;
    
    union {
        Touch::TouchEvent t;
        Keyboard::KeyEvent k;
        Mouse::MouseEvent m;
    } event;
        
    unsigned int hash;
    
    E_Input_Event type;
    
public:
    Vector2 *getLocation();
    Vector2 *getPreviousLocation();
    E_Input_Event eventType();
    int character();
    
    unsigned int identifier();
    
    inline bool operator==(const inputEvent& e) const;

    inputEvent(Keyboard::KeyEvent evt, int key);
    inputEvent(Touch::TouchEvent evt, int x, int y, unsigned int hash);
    inputEvent(Mouse::MouseEvent evt, int x, int y);
};

#endif /* defined(__SpaceSim__inputEvent__) */

#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>

class Thing;



class Component {
protected: 
    std::weak_ptr<Thing> thing;

public: 
    Component(std::weak_ptr<Thing> h) : thing(h) {
        setup();
    }
    
    virtual ~Component() {}
    virtual void setup();

};

#endif

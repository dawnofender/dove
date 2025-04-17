#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>

class Thing;



class Component {
protected: 
    std::shared_ptr<Thing> host;

public: 
    virtual ~Component() = default;
    virtual void update();
};

#endif

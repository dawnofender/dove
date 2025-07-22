#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "component.hpp"
#include "btBulletDynamicsCommon.h"
#include "../thingy.hpp"


class RigidBody : public Component {
CLASS_DECLARATION(RigidBody)
private:
    Thingy* host;
protected:
    btDefaultMotionState* motionstate;
public:
    float mass;

    RigidBody(std::string && initialValue, Thingy *h, float m);

};


#endif

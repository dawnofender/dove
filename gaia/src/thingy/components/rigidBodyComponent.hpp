#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "component.hpp"
#include "physicsComponent.hpp"
#include "btBulletDynamicsCommon.h"
#include "../thingy.hpp"


class RigidBody : public Component {
CLASS_DECLARATION(RigidBody)
private:
    Thingy* host;
    static inline std::vector<RigidBody *> rigidBodies;
protected:
    btDefaultMotionState* motionstate;
public:
    float mass;

    RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float m);

};


#endif

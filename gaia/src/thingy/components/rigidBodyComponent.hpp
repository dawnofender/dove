#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "component.hpp"
#include "transformComponent.hpp"
#include "physicsComponent.hpp"
#include "btBulletDynamicsCommon.h"
#include "../thingy.hpp"

class RigidBody : public Component {
CLASS_DECLARATION(RigidBody)
friend class Physics;
private:
    Thingy* host;
    Transform *transform;
    static inline std::vector<RigidBody *> rigidBodies;
protected:
    btTransform bulletTransform;
    btDefaultMotionState* motionstate;
    btRigidBody *bulletRigidBody;

public:
    float mass;

    RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float m);

    void syncFromTransform();
    void syncToTransform();
    static void syncFromTransforms();
    static void syncToTransforms();
};


#endif

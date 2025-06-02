#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "component.hpp"
#include "colliderComponent.hpp"
#include "transformComponent.hpp"
#include "physicsComponent.hpp"
#include "btBulletDynamicsCommon.h"
#include "../thingy/thingy.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

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
    bool b_kinematic;
    bool b_static;

public:
    float mass;

    RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float m = 0, bool k = false, bool s = false);

    void syncFromTransform();
    void syncToTransform();
    static void syncFromTransforms();
    static void syncToTransforms();
};


#endif

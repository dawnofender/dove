#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include "../component.hpp"
#include "../transformComponent.hpp"
#include "physicsComponent.hpp"
#include "colliderComponent.hpp"
#include "btBulletDynamicsCommon.h"
#include "../../thingy/thingy.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// NOTE: bullet has serialization functions for rigidbodies, use them

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

public:
    RigidBody(std::string && initialValue = "RigidBody", Physics *physicsComponent = nullptr, Thingy *h = nullptr, float m = 0, bool k = false, bool s = false);

    void syncFromTransform();
    void syncToTransform();
    static void syncFromTransforms();
    static void syncToTransforms();

    float getFriction();
    float getRollingFriction();
    float getSpinningFriction();
    glm::vec3 getLinearVelocity();
    glm::vec3 getAngularVelocity();
    float getMass();

    void setFriction(float newFriction);
    void setRollingFriction(float newRollingFriction);
    void setSpinningFriction(float newRollingFriction);
    void setLinearVelocity(glm::vec3 newVelocity);
    void setAngularVelocity(glm::vec3 newVelocity);
    void setMass(float newMass);

    void addForce(glm::vec3 force);
    void addForce(glm::vec3 force, glm::vec3 offset);
};


#endif

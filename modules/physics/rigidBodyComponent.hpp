#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "btBulletDynamicsCommon.h"

#include "../dove/transformComponent.hpp"
#include "physicsComponent.hpp"
#include "colliderComponent.hpp"

// TODO: 
// - if physicsComponent isn't given, it should just use the host and climb up through the hierarchy looking for a thingy with a physics component
// - bullet has serialization functions for rigidbodies, use them

class RigidBody : public Component {
CLASS_DECLARATION(RigidBody)
friend class Physics;

private:
    Thingy *host = nullptr;
    Transform *transform = nullptr;
    Physics *physicsComponent = nullptr;
    static inline std::set<RigidBody *> rigidBodies;

protected:
    btDefaultMotionState* motionstate = nullptr;
    btRigidBody *bulletRigidBody = nullptr;
    std::vector<Collider*> colliders = {nullptr};
    bool b_kinematic;
    bool b_static;

public: 
    float mass = 0;

public:
    RigidBody(std::string && initialName = "RigidBody", Physics *p = nullptr, Thingy *h = nullptr, float m = 0, bool k = false, bool s = false);
    ~RigidBody();

    virtual void serialize(Archive& archive) override;
    virtual void init() override;

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
    glm::vec3 getCenterOfMass();
    glm::mat4 getCenterOfMassTransform();

    void setFriction(float newFriction);
    void setRollingFriction(float newRollingFriction);
    void setSpinningFriction(float newRollingFriction);
    void setLinearVelocity(glm::vec3 newVelocity);
    void setAngularVelocity(glm::vec3 newVelocity);
    void setMass(float newMass);
    void setBounciness(float newBounciness);

    void addForce(glm::vec3 force);
    void addForce(glm::vec3 force, glm::vec3 offset);
    
};


#endif

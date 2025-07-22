#ifndef RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYCOMPONENT_HPP

#include <set>
#include "btBulletDynamicsCommon.h"

#include "physicsComponent.hpp"
#include "colliderComponent.hpp"
#include <core/math.hpp>

// TODO: 
// - if physicsComponent isn't given, it should just use the host and climb up through the hierarchy looking for a thingy with a physics component
// - bullet has serialization functions for rigidbodies, use them

class Transform;


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
    Dove::Vector3 getLinearVelocity();
    Dove::Vector3 getAngularVelocity();
    float getMass();
    Dove::Vector3 getCenterOfMass();
    Dove::Matrix4 getCenterOfMassTransform();
    Physics* getPhysicsWorld();

    void setFriction(float newFriction);
    void setRollingFriction(float newRollingFriction);
    void setSpinningFriction(float newRollingFriction);
    void setLinearVelocity(Dove::Vector3 newVelocity);
    void setAngularVelocity(Dove::Vector3 newVelocity);
    void setMass(float newMass);
    void setBounciness(float newBounciness);

    void addForce(Dove::Vector3 force);
    void addForce(Dove::Vector3 force, Dove::Vector3 offset);
    
};


#endif

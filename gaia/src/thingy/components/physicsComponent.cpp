#include "physicsComponent.hpp"

CLASS_DEFINITION(Component, Physics)


Physics::Physics(std::string &&initialValue)
      : Component(std::move(initialValue)) {

    // Build the broadphase
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    // The world.
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

}

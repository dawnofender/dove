#include "physicsComponent.hpp"

#include <iostream>
#include <string>
#include <sstream>

CLASS_DEFINITION(Component, Physics)



Physics::Physics(std::string &&initialValue)
    : Component(std::move(initialValue)) {
    // Build the broadphase
    broadphase = new btDbvtBroadphase();
    
    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;
    
    // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
    
    // btVector3 localInertia(0, 0, 0);
    // btTransform groundTransform;
    // groundTransform.setIdentity();
    // groundTransform.setOrigin(btVector3(0, -50, 0));
    // btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
    // btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    // btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, groundShape, localInertia);
    // btRigidBody* body = new btRigidBody(rbInfo);
    // body->setFriction(1);
    // //add the body to the dynamics world
    // dynamicsWorld->addRigidBody(body);
}

Thingy* Physics::rayCast(glm::vec3 origin, glm::vec3 direction, float distance) {
    
    glm::vec3 out_end = origin + direction * distance;
    
    btCollisionWorld::ClosestRayResultCallback RayCallback(
      	btVector3(origin.x, origin.y, origin.z), 
    	  btVector3(out_end.x, out_end.y, out_end.z)
    );

    dynamicsWorld->rayTest(
      	btVector3(origin.x, origin.y, origin.z), 
    	  btVector3(out_end.x, out_end.y, out_end.z), 
    	  RayCallback
    );
    
    std::string message;
    if (RayCallback.hasHit()) {
	      return (Thingy*)RayCallback.m_collisionObject->getUserPointer();
    } else {
	      return nullptr;
    }

    std::cout << "raycast info:" << std::endl <<
        "start: " << origin.x << ", " << origin.y << ", " << origin.z << std::endl <<
        "end:   " << out_end.x << ", " << out_end.y << ", " << out_end.z << std::endl <<
        "dir:   " << direction.x << ", " << direction.y << ", " << direction.z << std::endl <<
        message << std::endl;

}

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
}

void Physics::addRigidBody(btRigidBody *rigidBody) {
    dynamicsWorld->addRigidBody(rigidBody);
}

void Physics::rayCast(glm::vec3 origin, glm::vec3 direction, float distance) {
    
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
        std::cout << "hit" << std::endl;
    	  std::ostringstream oss;
	      oss << "mesh " << (intptr_t)RayCallback.m_collisionObject->getUserPointer();
	      message = oss.str();
    } else {
	      message = "background";
    }

    std::cout << "raycast info:" << std::endl <<
        "start: " << origin.x << ", " << origin.y << ", " << origin.z << std::endl <<
        "end:   " << out_end.x << ", " << out_end.y << ", " << out_end.z << std::endl <<
        "dir:   " << direction.x << ", " << direction.y << ", " << direction.z << std::endl <<
        message << std::endl;

}

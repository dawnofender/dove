#include "physicsComponent.hpp"

#include <iostream>
#include <string>
#include <sstream>

CLASS_DEFINITION(Component, Physics)



Physics::Physics(std::string &&initialValue)
    : Component(std::move(initialValue)) {
    // Build the broadphase
	// filterCallback = new btOverlapFilterCallback();
    // pairCache = new btHashedOverlappingPairCache();
    // pairCache->setOverlapFilterCallback(filterCallback);
    // broadphase = new btDbvtBroadphase(pairCache);

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

RayCastInfo Physics::rayCast(glm::vec3 origin, glm::vec3 direction, float distance) {
    
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
    
    
	  return RayCastInfo {
        RayCallback.hasHit(),
        (Thingy*)RayCallback.m_collisionObject->getUserPointer(),
        glm::vec3(
            RayCallback.m_hitPointWorld.x(),
            RayCallback.m_hitPointWorld.y(),
            RayCallback.m_hitPointWorld.z()
        ),
        glm::vec3(
            RayCallback.m_hitNormalWorld.x(),
            RayCallback.m_hitNormalWorld.y(),
            RayCallback.m_hitNormalWorld.z()
        )
    };
}

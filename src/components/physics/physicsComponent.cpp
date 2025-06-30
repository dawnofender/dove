#include "physicsComponent.hpp"


#include <iostream>
#include <string>
#include <sstream>
#include "rigidBodyComponent.hpp"

CLASS_DEFINITION(Component, Physics)



Physics::Physics(std::string &&initialValue)
    : Component(std::move(initialValue)) {}

Physics::~Physics() {
    physicsWorlds.erase(std::remove(physicsWorlds.begin(), physicsWorlds.end(), this), physicsWorlds.end());
}


void Physics::init() {
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
    
    // tick callback 
    //  - this is where bullet3 tells us info like: 
    //      - how many collisions occured?
    //      - what collided with what? 
    dynamicsWorld->setInternalTickCallback(OnSimulationTick);

    // Debug drawer
    dynamicsWorld->setDebugDrawer(&mydebugdrawer);

    physicsWorlds.push_back(this);

    // i'm so sure there is a better way to do this, but the tick callback has to be static for whatever reason, but I also want it to know which physics component is active, sooo we're using an unordered map to get that information for now
    worldMap.insert(std::make_pair(dynamicsWorld, this));
}

void Physics::OnSimulationTick(btDynamicsWorld *world, btScalar timeStep) { 
    Physics* physicsComponent = worldMap.at((btDiscreteDynamicsWorld*)world);
    physicsComponent->collisionMap.clear();
    physicsComponent->collisions.clear();

    int numManifolds = world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* objectA = contactManifold->getBody0();
        const btCollisionObject* objectB = contactManifold->getBody1();
        
        Thingy* thingyA = static_cast<Thingy*>(objectA->getUserPointer());
        Thingy* thingyB = static_cast<Thingy*>(objectB->getUserPointer());
        
        const int numContacts = contactManifold->getNumContacts();
        std::vector<std::shared_ptr<CollisionInfo>> collisionInfoObjects;

        for (int j = 0; j < numContacts; j++) {
            btManifoldPoint& point = contactManifold->getContactPoint(j);

            if (point.getDistance() < 0.0f ) {
                const btVector3& bulletPointA = point.getPositionWorldOnA();
                const btVector3& bulletPointB = point.getPositionWorldOnB();
                const btVector3& bulletNormalOnB = point.m_normalWorldOnB;
                
                const glm::vec3 pointA(bulletPointA.x(), bulletPointA.y(), bulletPointA.z());
                const glm::vec3 pointB;
                const glm::vec3 normalOnB;
                std::shared_ptr<CollisionInfo> collisionInfo = std::make_shared<CollisionInfo>(thingyA, thingyB, pointA, pointB, normalOnB);
                physicsComponent->collisions.push_back(collisionInfo);
                collisionInfoObjects.push_back(collisionInfo);
            }
        }

        physicsComponent->collisionMap.insert(std::make_pair(thingyA, collisionInfoObjects));
        physicsComponent->collisionMap.insert(std::make_pair(thingyB, collisionInfoObjects));
    }
}

void Physics::debugDrawWorld() {
    mydebugdrawer.SetMatrices(Camera::getViewMatrix(), Camera::getProjectionMatrix());
    dynamicsWorld->debugDrawWorld();
}
void Physics::debugDrawAll() {
    mydebugdrawer.SetMatrices(Camera::getViewMatrix(), Camera::getProjectionMatrix());
    for (auto && world : physicsWorlds) {
        world->debugDrawWorld();
    }
}

std::vector<std::shared_ptr<CollisionInfo>> Physics::getCollisionInfo(Thingy* thingy) {
  	// Searching for element with key
    if (collisionMap.find(thingy) == collisionMap.end())
        return {nullptr};
    else
        return collisionMap.at(thingy);
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
    
    RayCastInfo rayCastInfo;

    if ((rayCastInfo.hasHit = RayCallback.hasHit())) {
        rayCastInfo.thingy = (Thingy*)RayCallback.m_collisionObject->getUserPointer();
        rayCastInfo.position = glm::vec3(
            RayCallback.m_hitPointWorld.x(),
            RayCallback.m_hitPointWorld.y(),
            RayCallback.m_hitPointWorld.z()
        );

        rayCastInfo.normal = glm::vec3(
            RayCallback.m_hitNormalWorld.x(),
            RayCallback.m_hitNormalWorld.y(),
            RayCallback.m_hitNormalWorld.z()
        );
    }
    return rayCastInfo;
}

void Physics::simulateAll() {
    std::cout << "syncing from transforms" << std::endl;
    RigidBody::syncFromTransforms();
    for (auto && world : physicsWorlds) {
        std::cout << "simulating world" << std::endl;
        world->simulate();
    }
    
    std::cout << "syncing transforms" << std::endl;
    RigidBody::syncToTransforms();
    std::cout << "syncing transforms: done" << std::endl;
}

void Physics::simulate() {
    time = glfwGetTime();
    // deltaTime = time - lastTime;
    deltaTime = 0.01f;
    lastTime = time;

    std::cout << "stepping simulation" << std::endl;
    dynamicsWorld->stepSimulation(deltaTime);
    dynamicsWorld->performDiscreteCollisionDetection();
}

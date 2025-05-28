#include "rigidBodyComponent.hpp"
#include "colliderComponent.hpp"
#include "transformComponent.hpp"
#include <glm/glm.hpp>

#include <iostream>

CLASS_DEFINITION(Component, RigidBody)

RigidBody::RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float m) 
    : Component(std::move(initialValue)), host(h), mass(m) {
    
    // get transform information
    Transform transform = host->getComponent<Transform>();
    glm::vec3 position = transform.position;
    glm::vec4 rotation = transform.rotation;

    // get shape of collider
    // FIX: handle no collider found
    btCollisionShape* colliderShape = host->getComponent<Collider>().collisionShape;
    std::cout << host->getComponent<Collider>().value << std::endl;

    motionstate = new btDefaultMotionState( btTransform(
        btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w), 
        btVector3(position.x, position.y, position.z)
    ));


    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
    	mass,                 // mass, in kg. 0 -> Static object, will never move.
    	motionstate,
    	colliderShape,        // collision shape from collider component
    	btVector3(0,0,0)      // local inertia
    );
    
    btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
    
    rigidBody->setUserPointer((void*)host);
    physicsComponent->dynamicsWorld->addRigidBody(rigidBody);
    // physicsComponent->addRigidBody(rigidBody);
    std::cout << "created rigidbody" << std::endl;
}

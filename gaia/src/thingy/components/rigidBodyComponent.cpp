#include "rigidBodyComponent.hpp"
#include "colliderComponent.hpp"
#include "transformComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

CLASS_DEFINITION(Component, RigidBody)


RigidBody::RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float mass = 0) 
    : Component(std::move(initialValue)), host(h) {
    
    // get transform information
    transform = &host->getComponent<Transform>();

    glm::vec3 position = transform->getPosition();
    // glm::vec4 rotation = transform->getRotation();

    //     btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w), 
    //     btVector3(position.x, position.y, position.z)
    // );
    bulletTransform.setIdentity();
    bulletTransform.setOrigin(btVector3(position.x, position.y, position.z));

    // get shape of collider
    // FIX: handle no collider found
    btCollisionShape* colliderShape = host->getComponent<Collider>().collisionShape;

    motionstate = new btDefaultMotionState(bulletTransform);


    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
    	  mass,                 // mass, in kg. 0 -> Static object, will never move.
    	  motionstate,
    	  colliderShape,        // collision shape from collider component
    	  btVector3(0,0,0)      // local inertia
    );
    
    bulletRigidBody = new btRigidBody(rigidBodyCI);
    bulletRigidBody->setUserPointer((void*)host);
    physicsComponent->dynamicsWorld->addRigidBody(bulletRigidBody);
    std::cout << "created rigidbody" << std::endl;

    rigidBodies.push_back(this);
}

void RigidBody::syncFromTransform() {
    // get transform matrix from transform component
    float transformMatrix[16];
    memcpy(transformMatrix, (void*)glm::value_ptr(transform->getMatrix()), 16*sizeof(GLfloat));
    
    bulletTransform.setFromOpenGLMatrix(transformMatrix);
    bulletRigidBody->setWorldTransform(bulletTransform);
}

void RigidBody::syncToTransform() {
    float bulletTransformMatrix[16];
    bulletRigidBody->getCenterOfMassTransform().getOpenGLMatrix(bulletTransformMatrix);
    transform->setMatrix(glm::make_mat4(bulletTransformMatrix));
}

void RigidBody::syncFromTransforms() {
    for (auto &rigidBody : rigidBodies) {
        rigidBody->syncFromTransform();
    }
}

void RigidBody::syncToTransforms() {
    for (auto &rigidBody : rigidBodies) {
        rigidBody->syncToTransform();
    }
}

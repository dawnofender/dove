#include "rigidBodyComponent.hpp"

CLASS_DEFINITION(Component, RigidBody)


RigidBody::RigidBody(std::string && initialValue, Physics *physicsComponent, Thingy *h, float m, bool k, bool s) 
    : Component(std::move(initialValue)), host(h), mass(m), b_kinematic(k), b_static(s) {
    
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
    btVector3 localInertia;

    if (mass != 0.f) colliderShape->calculateLocalInertia(mass, localInertia);

    bulletRigidBody = new btRigidBody( btRigidBody::btRigidBodyConstructionInfo(
    	  mass,
    	  motionstate,
    	  colliderShape,
    	  localInertia
    ));
    
	// bulletRigidBody->setContactStiffnessAndDamping(300, 10);

    bulletRigidBody->setUserPointer((void*)host);
    if (b_kinematic) bulletRigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
    if (b_static)    bulletRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    
    physicsComponent->dynamicsWorld->addRigidBody(bulletRigidBody);

    rigidBodies.push_back(this);
}

void RigidBody::addForce(glm::vec3 force) {
    bulletRigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void RigidBody::addForce(glm::vec3 force, glm::vec3 offset) {
    bulletRigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(offset.x, offset.y, offset.z));
}

void RigidBody::syncFromTransform() {
    float bulletTransformMatrix[16];
    // get transform matrix from transform component, but with reset scale
    glm::mat4 oglTransformMatrix = glm::scale(transform->getMatrix(), glm::vec3(1) / transform->getScale());
    memcpy(bulletTransformMatrix, (void*)glm::value_ptr(oglTransformMatrix), 16*sizeof(GLfloat));
    
    bulletTransform.setFromOpenGLMatrix(bulletTransformMatrix);
    bulletRigidBody->setWorldTransform(bulletTransform);
}

void RigidBody::syncToTransform() {
    float bulletTransformMatrix[16];
    bulletRigidBody->getCenterOfMassTransform().getOpenGLMatrix(bulletTransformMatrix);
    glm::mat4 oglTransformMatrix = glm::make_mat4(bulletTransformMatrix);
    transform->setPosition(glm::vec3(oglTransformMatrix[3]));
    transform->setOrientation(glm::quat_cast(oglTransformMatrix));
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

glm::vec3 RigidBody::getLinearVelocity() {
	  btVector3 v = bulletRigidBody->getLinearVelocity();
    return glm::vec3(v.x(), v.y(), v.z()); 
}

glm::vec3 RigidBody::getAngularVelocity() {
	  btVector3 v = bulletRigidBody->getAngularVelocity();
	  return glm::vec3(v.x(), v.y(), v.z());
}

float RigidBody::getMass() {
    return bulletRigidBody->getMass();
}

void RigidBody::setMass(float newMass) {
    // bulletRigidBody->setMass(newMass);
}

float RigidBody::getFriction() {
    return bulletRigidBody->getFriction();
}

float RigidBody::getRollingFriction() {
    return bulletRigidBody->getRollingFriction();
}

float RigidBody::getSpinningFriction() {
    return bulletRigidBody->getSpinningFriction();
}

void RigidBody::setFriction(float newFriction) {
    bulletRigidBody->setFriction(newFriction);
}

void RigidBody::setRollingFriction(float newRollingFriction) {
    bulletRigidBody->setRollingFriction(newRollingFriction);
}

void RigidBody::setSpinningFriction(float newSpinningFriction) {
    bulletRigidBody->setSpinningFriction(newSpinningFriction);
}


void RigidBody::setLinearVelocity(glm::vec3 v) {
    bulletRigidBody->setLinearVelocity(btVector3(v.x, v.y, v.z));
}

void RigidBody::setAngularVelocity(glm::vec3 v) {
    bulletRigidBody->setAngularVelocity(btVector3(v.x, v.y, v.z));
}



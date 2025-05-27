#include "rigidBodyComponent.hpp"
#include "colliderComponent.hpp"
#include "transformComponent.hpp"
#include <glm/glm.hpp>

CLASS_DEFINITION(Component, RigidBody)

RigidBody::RigidBody(std::string && initialValue, Thingy *h, float m) 
    : Component(std::move(initialValue)), host(h), mass(m) {
    
    // get transform information
    glm::vec3 position = host->getComponent<Transform>().position;
    glm::vec4 rotation = host->getComponent<Transform>().rotation;

    // get shape of collider
    btCollisionShape* colliderShape = host->getComponent<Collider>().collisionShape;

    // motionstate = new btDefaultMotionState( btTransform(
	   //    bt  Quaternion(rotation.x, rotation.y, rotation.z, rotation.w), 
	   //    bt  Vector3(position.x, position.y, position.z)
    

    // btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
    // 	  mass,                 // mass, in kg. 0 -> Static object, will never move.
    // 	  motionstate,
    // 	  colliderShape,        // collision shape from collider component
    // 	  btVector3(0,0,0)      // local inertia
    // );
}

#include "sphereColliderComponent.hpp"

CLASS_DEFINITION(Collider, SphereCollider)
        
SphereCollider::SphereCollider(std::string &&initialValue, float radius)
        : Collider(std::move(initialValue), new btSphereShape(radius)) {}

// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

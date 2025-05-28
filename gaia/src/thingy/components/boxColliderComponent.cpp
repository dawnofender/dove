#include "boxColliderComponent.hpp"

CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialValue, float dx, float dy, float dz)
        : Collider(std::move(initialValue), new btBoxShape(btVector3(dx, dy, dz))) {}

// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

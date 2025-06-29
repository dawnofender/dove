#include "sphereColliderComponent.hpp"

CLASS_DEFINITION(Collider, SphereCollider)

SphereCollider::SphereCollider(std::string &&initialValue)
    : Collider(std::move(initialValue)) {}

SphereCollider::SphereCollider(std::string &&initialValue, float r)
    : Collider(std::move(initialValue)), radius(r) {}

void SphereCollider::load() {
    collisionShape = new btSphereShape(radius);
}

// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

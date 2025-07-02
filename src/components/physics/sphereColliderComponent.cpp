#include "sphereColliderComponent.hpp"

CLASS_DEFINITION(Collider, SphereCollider)

SphereCollider::SphereCollider(std::string &&initialValue, float r)
    : Collider(std::move(initialValue)), radius(r) {}

void SphereCollider::serialize(Archive& archive) {
    archive & radius;
}

void SphereCollider::init() {
    Collider::init();
    collisionShape = new btSphereShape(radius);
}

// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

#include "boxColliderComponent.hpp"
#include "../transform/transformComponent.hpp"
CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialName, Dove::Vector3 d) 
    : Collider(std::move(initialName)), dimensions(d) {}

void BoxCollider::serialize(Archive& archive) {
    archive & dimensions;
}

void BoxCollider::init() {
    Collider::init();
    collisionShape = new btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z));
}
// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

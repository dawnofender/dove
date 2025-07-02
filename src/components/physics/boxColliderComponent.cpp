#include "boxColliderComponent.hpp"
#include "../transformComponent.hpp"
CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialValue, glm::vec3 d) 
    : Collider(std::move(initialValue)), dimensions(d) {}

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

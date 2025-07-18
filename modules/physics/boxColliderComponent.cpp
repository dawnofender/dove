#include "boxColliderComponent.hpp"
#include "../dove/transformComponent.hpp"
CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialName, glm::vec3 d) 
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

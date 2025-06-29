#include "boxColliderComponent.hpp"
#include "../transformComponent.hpp"
CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialValue) 
    : Collider(std::move(initialValue)) {}

BoxCollider::BoxCollider(std::string &&initialValue, glm::vec3 d) 
    : Collider(std::move(initialValue)), dimensions(d) {}

void BoxCollider::load() {
    collisionShape = new btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z));
}
// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

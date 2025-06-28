#include "boxColliderComponent.hpp"
#include "../transformComponent.hpp"
CLASS_DEFINITION(Collider, BoxCollider)
        
BoxCollider::BoxCollider(std::string &&initialValue, glm::vec3 d) 
    : Collider(std::move(initialValue), new btBoxShape(btVector3(d.x, d.y, d.z))) {}

// SphereCollider::setRadius(float radius) {
//     collisionShape
// }

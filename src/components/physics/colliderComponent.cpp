#include "colliderComponent.hpp"

CLASS_DEFINITION(Component, Collider)

Collider::Collider( std::string && initialValue, btCollisionShape* shape)
    : Component(std::move( initialValue )), collisionShape(shape) { 
}

#include "colliderComponent.hpp"

CLASS_DEFINITION(Component, Collider)

Collider::Collider( std::string && initialName)
    : Component(std::move(initialName)) {}

Collider::Collider( std::string && initialName, btCollisionShape* shape)
    : Component(std::move( initialName )), collisionShape(shape) {}

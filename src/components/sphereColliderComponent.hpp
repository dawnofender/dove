#ifndef SPHERECOLLIDERCOMPONENT_HPP
#define SPHERECOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"

class SphereCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
public: 
    SphereCollider() = default;
    SphereCollider(std::string &&initialValue, float radius);

    void setRadius(float r);

};

#endif


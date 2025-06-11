#ifndef SPHERECOLLIDERCOMPONENT_HPP
#define SPHERECOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"

class SphereCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
public: 
    SphereCollider(std::string &&initialValue = "SphereCollider", float radius = 0.5f);

    void setRadius(float r);

};

#endif


#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
public: 
    BoxCollider(std::string &&initialValue, float dx, float dy, float dz);
};

#endif


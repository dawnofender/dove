#ifndef SPHERECOLLIDERCOMPONENT_HPP
#define SPHERECOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"

class SphereCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
public: 
    SphereCollider(std::string &&initialValue = "SphereCollider");
    SphereCollider(std::string &&initialValue, float r);

    void setRadius(float r);
    virtual void load() override;

private:
    float radius;
};

#endif


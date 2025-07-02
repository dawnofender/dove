#ifndef SPHERECOLLIDERCOMPONENT_HPP
#define SPHERECOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"

class SphereCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
public: 
    SphereCollider(std::string &&initialValue = "SphereCollider", float r = 0.5f);
    
    virtual void serialize(Archive& archive) override;
    virtual void init() override;

    void setRadius(float r);

private:
    float radius;
};

#endif


#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"
#include <glm/glm.hpp>
#include "../../thingy/thingy.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(BoxCollider)
public: 
    BoxCollider(std::string &&initialValue = "BoxCollider");
    BoxCollider(std::string &&initialValue, glm::vec3 d);
    virtual void load() override;
private: 
    glm::vec3 dimensions;
};

#endif


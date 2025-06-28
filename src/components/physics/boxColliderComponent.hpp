#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"
#include <glm/glm.hpp>
#include "../../thingy/thingy.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(BoxCollider)
public: 
    BoxCollider(std::string &&initialValue = "BoxCollider", glm::vec3 d = {0.5, 0.5, 0.5});
};

#endif


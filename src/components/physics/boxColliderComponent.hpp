#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"
#include <glm/glm.hpp>
#include "../../thingy/thingy.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(BoxCollider)
private:
    Thingy* host;
public: 
    BoxCollider(std::string &&initialValue = "BoxCollider", Thingy *h = nullptr, glm::vec3 d = {0.5, 0.5, 0.5});
};

#endif


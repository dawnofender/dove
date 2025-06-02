#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"
#include <glm/glm.hpp>
#include "../thingy/thingy.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(SphereCollider)
private:
    Thingy* host;
public: 
    BoxCollider(std::string &&initialValue, Thingy *h, glm::vec3 d = {0.5, 0.5, 0.5});
};

#endif


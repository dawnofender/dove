#ifndef BOXCOLLIDERCOMPONENT_HPP
#define BOXCOLLIDERCOMPONENT_HPP

#include "colliderComponent.hpp"
#include <glm/glm.hpp>
#include "../../thingy/thingy.hpp"

class BoxCollider : public Collider {
CLASS_DECLARATION(BoxCollider)
public: 
    BoxCollider(std::string &&initialName = "BoxCollider", glm::vec3 d = {1, 1, 1});
    virtual void serialize(Archive& archive) override;
    virtual void init() override;

private: 
    glm::vec3 dimensions;
};

#endif


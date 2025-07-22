#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include "component.hpp"


class Transform : public Component {
CLASS_DECLARATION(Transform)
public: 
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;

public: 
    Transform(std::string && initialValue)
        : Component(std::move(initialValue)) {
    }
};


#endif

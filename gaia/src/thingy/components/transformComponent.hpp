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
    Transform(std::string && initialValue, glm::vec3 p = {0, 0, 0}, glm::vec4 r = {0, 0, 0, 0}, glm::vec3 s = {0, 0, 0})
        : Component(std::move(initialValue)), position(p), rotation(r), scale(s) {
    }
};


#endif

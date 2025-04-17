#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include "component.hpp"


class Transform : public Component {
public: 
    glm::vec3 position;
    // glm::quat rotation;
    glm::vec3 scale;
};


#endif

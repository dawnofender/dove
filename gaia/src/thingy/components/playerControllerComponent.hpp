#ifndef PLAYERCONTROLLERCOMPONENT_HPP
#define PLAYERCONTROLLERCOMPONENT_HPP

#include "component.hpp"
#include <glm/glm.hpp>


class PlayerController: public Component {
CLASS_DECLARATION(PlayerController)
private: 
    glm::vec3 position;

public: 
    PlayerController(std::string && initialValue)
        : Component(std::move(initialValue)) {
    }

    void teleport(glm::vec3 pos) {
        position = pos;
    }

    glm::vec3 getPosition() {
        return position;
    }

};


#endif

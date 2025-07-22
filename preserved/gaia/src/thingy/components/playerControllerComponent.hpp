#ifndef PLAYERCONTROLLERCOMPONENT_HPP
#define PLAYERCONTROLLERCOMPONENT_HPP

#include "component.hpp"
#include "transformComponent.hpp"
#include <src/thingy/thingy.hpp>
#include <glm/glm.hpp>

class PlayerController: public Component {
CLASS_DECLARATION(PlayerController)
private: 
    Thingy* host;
    Transform* transformComp;

public: 
    PlayerController(std::string && initialValue, Thingy* h)
        : Component(std::move(initialValue)), host(h) {
        
        // FIX: breaks if transform doesn't exist
        transformComp = &host->getComponent<Transform>();
    }

    void teleport(glm::vec3 pos) {
        transformComp->position = pos;
    }

    glm::vec3 getPosition() {
        return transformComp->position;
    }

};


#endif

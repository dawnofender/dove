#ifndef PLAYERCONTROLLERCOMPONENT_HPP
#define PLAYERCONTROLLERCOMPONENT_HPP

#include "updatableComponent.hpp"
#include "transformComponent.hpp"
#include <src/thingy/thingy.hpp>
#include <glm/glm.hpp>

class PlayerController: public UpdatableComponent {
CLASS_DECLARATION(PlayerController)
private: 
    Thingy* host;
    Thingy* camera;
    Transform* playerTransform;
    Transform* cameraTransform;

public: 
    PlayerController(std::string && initialValue = "PlayerController", Thingy* h = nullptr, Thingy* c = nullptr, float s = 4);
    
    void update();

    float speed;
};


#endif

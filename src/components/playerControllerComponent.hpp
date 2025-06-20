#ifndef PLAYERCONTROLLERCOMPONENT_HPP
#define PLAYERCONTROLLERCOMPONENT_HPP

#include "updatableComponent.hpp"
#include "transformComponent.hpp"
#include "physics/rigidBodyComponent.hpp"
#include <src/thingy/thingy.hpp>
#include <glm/glm.hpp>

class PlayerController: public UpdatableComponent {
CLASS_DECLARATION(PlayerController)
private: 
    Thingy* host;
    Thingy* camera;
    RigidBody* playerRigidBody;
    Transform* cameraTransform;
    Physics* physicsComponent;
    int jumpTimer = 0;
    float pitch;
    float yaw;

public: 
    PlayerController(std::string && initialValue = "PlayerController", Physics* p = nullptr, Thingy* h = nullptr, Thingy* c = nullptr, RigidBody* r = nullptr, Transform* t = nullptr, float s = 1000, float j = 1500, float m = 1.5f, float ms = 0.01);
    
    void update();
    
    float mouseSensitivity;
    float walkSpeed;
    float jumpStrength;
    float maxIncline;
};


#endif

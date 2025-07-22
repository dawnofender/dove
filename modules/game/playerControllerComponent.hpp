#ifndef PLAYERCONTROLLERCOMPONENT_HPP
#define PLAYERCONTROLLERCOMPONENT_HPP

#include "../../core/component/updatableComponent.hpp"
#include "../physics/rigidBodyComponent.hpp"

#include "../transform/transformComponent.hpp"

#include <core/math.hpp>

class PlayerController: public UpdatableComponent {
CLASS_DECLARATION(PlayerController)
public: 
    PlayerController(std::string && initialName = "PlayerController", Physics* p = nullptr, Thingy* h = nullptr, Thingy* c = nullptr, RigidBody* r = nullptr, Transform* t = nullptr, float s = 10000, float j = 1500, float m = 1.5f, float ms = 0.01);
    
    virtual void serialize(Archive& archive) override;

    virtual void update() override;
    
    void setCameraTransform(Transform* transform);
    void setRigidBody(RigidBody* rigidbody);

    float mouseSensitivity;
    float walkSpeed;
    float jumpStrength;
    float maxIncline;

private: 
    Thingy* host;
    RigidBody* playerRigidBody;
    Transform* cameraTransform;
    Physics* physicsComponent;
    float jumpTimer = 0.f;
    float jumpCooldown = 0.1f; // in seconds
    float pitch = 0.f;
    float yaw = 3.141592f;
    
    // TODO: Move this, especially reseting cursorpos, to a separate component
    Dove::Vector2 calculateMouseMovement(Window& window) const;

};


#endif

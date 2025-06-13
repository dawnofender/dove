#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"
#include <glm/gtx/euler_angles.hpp>

CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(std::string && initialValue, Physics* p, Thingy* h, Thingy* c, RigidBody* r, Transform* t, float s) 
    : UpdatableComponent(std::move(initialValue)), physicsComponent(p), host(h), camera(c), playerRigidBody(r), cameraTransform(t), speed(s) {}


void PlayerController::update() {
    if (!physicsComponent) return;
    if (!Camera::activeWindow) return;
    if (!playerRigidBody) {
        if (!host) return;
        playerRigidBody = &host->getComponent<RigidBody>();
        return;
    }
    if (!cameraTransform) {
        if (!camera) return;
        if (!(cameraTransform = &camera->getComponent<Transform>()))
            cameraTransform = &camera->addComponent<Transform>("Transform", camera);
        return;
    }
    
    glm::vec3 up(0, 1, 0);
    // if this doesn't work, rotate so the y axis points in the x direction and then extract
    glm::vec3 angle;
    glm::extractEulerAngleXYZ(cameraTransform->getMatrix(), angle.x, angle.y, angle.z);
    glm::vec3 force;
    
    // get direction from key inputs
    glm::mat4 direction = glm::translate(
        glm::mat4(0),
        glm::vec3{
            // each key press (W, A, S, D) is read as 0 or 1
            // subtract backwards from forwards 
            (glfwGetKey( Camera::activeWindow, GLFW_KEY_W ) == GLFW_PRESS) - (glfwGetKey( Camera::activeWindow, GLFW_KEY_S ) == GLFW_PRESS),
            // zero up for now
            0,
            // subtract left from right
            (glfwGetKey( Camera::activeWindow, GLFW_KEY_D ) == GLFW_PRESS) - (glfwGetKey( Camera::activeWindow, GLFW_KEY_A ) == GLFW_PRESS),
        }
    );
    
    // rotate direction to face forwards
    direction = glm::rotate(direction, angle.y, up);

    force = speed * glm::vec3(direction[3]);

    
    if (grounded && glfwGetKey( Camera::activeWindow, GLFW_KEY_SPACE ) == GLFW_PRESS) {
        force += glm::vec3(0, 100, 0);
        grounded = false;
    } else {
        Transform* playerTransform = &host->getComponent<Transform>();
        if (playerTransform) 
            auto rayCastInfo = physicsComponent->rayCast(playerTransform->getPosition(), glm::vec3(0, -1, 0), 1);

            // grounded = 
            //     physicsComponent->rayCast(
            //         playerTransform->getPosition(), 
            //         glm::vec3(0, -1, 0),
            //         0.6
            //     ).hasHit;
    }

    playerRigidBody->addForce(force);

    // do camera transform = lookat()
    
}


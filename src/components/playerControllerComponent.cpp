#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"

CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(std::string && initialValue, Thingy* h, Thingy* c, float s) 
    : UpdatableComponent(std::move(initialValue)), host(h), camera(c), speed(s) {

    playerTransform = &host->getComponent<Transform>();
    cameraTransform = &host->getComponent<Transform>();

}

void PlayerController::update() {
    if (!playerTransform) {
       // playerTransform = &host->addComponent<Transform>("Transform", host);
        return;
    }
    if (!cameraTransform) {
        // cameraTransform = &camera->addComponent<Transform>("Transform", camera);
        return;
    }

    glm::vec2 direction = speed * glm::normalize(
        glm::vec2{
            // each key press (W, A, S, D) is read as 0 or 1
            // subtract backwards from forwards 
            (glfwGetKey( Camera::activeWindow, GLFW_KEY_W ) == GLFW_PRESS) - (glfwGetKey( Camera::activeWindow, GLFW_KEY_S ) == GLFW_PRESS),
            // subtract left from right
            (glfwGetKey( Camera::activeWindow, GLFW_KEY_D ) == GLFW_PRESS) - (glfwGetKey( Camera::activeWindow, GLFW_KEY_A ) == GLFW_PRESS),
    });
    
    // direction = glm::
    // do camera transform = lookat()
    
}


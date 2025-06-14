#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"
#include <glm/gtx/euler_angles.hpp>

CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(std::string && initialValue, Physics* p, Thingy* h, Thingy* c, RigidBody* r, Transform* t, float s, float j) 
    : UpdatableComponent(std::move(initialValue)), physicsComponent(p), host(h), camera(c), playerRigidBody(r), cameraTransform(t), speed(s), jumpStrength(j) {}


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
    // if this doesn't work, rotate so the y axis points in the x direction and then extract
    glm::vec3 angle;
    glm::mat4 viewMatrix = Camera::getViewMatrix();
    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec3 forward = -glm::vec3(invView[2]);
    glm::vec3 up(0, 1, 0);
    glm::vec3 right = glm::cross(forward, up);

    // glm::extractEulerAngleXYZ(viewMatrix, angle.x, angle.y, angle.z);
    glm::vec3 force;

    // glm::vec3 forward(sin(angle.y), 0, cos(angle.y));
    
    // get direction from key inputs
    glm::vec3 input = {0, 0, 0};
    if (glfwGetKey( Camera::activeWindow, GLFW_KEY_W ) == GLFW_PRESS)
        input += forward;
    if (glfwGetKey( Camera::activeWindow, GLFW_KEY_S ) == GLFW_PRESS)
        input -= forward;
    if (glfwGetKey( Camera::activeWindow, GLFW_KEY_D ) == GLFW_PRESS) 
        input += right;
    if (glfwGetKey( Camera::activeWindow, GLFW_KEY_A ) == GLFW_PRESS)
        input -= right;

    // normalize causes NAN if the length is zero, lets avoid this
    if (glm::length2(input) > 0) {
        // flatten and normalize input dir (for no flying movement)
        input = glm::normalize(input * glm::vec3(1, 0, 1));
    }
    
    force = speed * input;

    
    Transform* playerTransform = &host->getComponent<Transform>();
    if (
            playerTransform &&
            // has it been a bit since the last jump?
            jumpTimer >= 0 &&
            // are we pressing space?
            glfwGetKey( Camera::activeWindow, GLFW_KEY_SPACE ) == GLFW_PRESS &&
            // are we touching the ground?
            physicsComponent->rayCast( playerTransform->getPosition(), glm::vec3(0, -1, 0), 0.501 ).hasHit
    ) {
        RayCastInfo rayCastInfo = physicsComponent->rayCast( playerTransform->getPosition(), glm::vec3(0, -1, 0), 0.501 );
        if (rayCastInfo.hasHit) {
            force += glm::vec3(0, jumpStrength, 0);
            jumpTimer = 200;
            // apply force to the thing we jumped off of
            RigidBody *platformRigidBody = &rayCastInfo.thingy->getComponent<RigidBody>();
            if (platformRigidBody) {
                platformRigidBody->addForce(glm::vec3(0, -jumpStrength, 0));
            }
        }
    } else if (jumpTimer > 0) 
        jumpTimer--;

    playerRigidBody->addForce(force);
    playerRigidBody->setAngularVelocity(glm::vec3(0, 0, 0));


    // do camera transform = lookat()
    
}


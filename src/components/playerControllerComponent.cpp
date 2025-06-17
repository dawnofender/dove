#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"
#include <glm/gtx/euler_angles.hpp>

CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(std::string && initialValue, Physics* p, Thingy* h, Thingy* c, RigidBody* r, Transform* t, float s, float j, float m) 
    : UpdatableComponent(std::move(initialValue)), physicsComponent(p), host(h), camera(c), playerRigidBody(r), cameraTransform(t), speed(s), jumpStrength(j), maxIncline(m) {}


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
    
    // jump logic

    if (
        // are we pressing space?
        glfwGetKey( Camera::activeWindow, GLFW_KEY_SPACE ) == GLFW_PRESS &&
        // has it been a bit since the last jump?
        jumpTimer >= 0
    ) {
        // are we grounded? 
        // whats the ground? 
        auto collisions = physicsComponent->getCollisionInfo(host);
        Thingy* platform;
        // check each collision and compare to find a valid incline
        float bestIncline = maxIncline;
        for (auto&& collisionInfo : collisions) {
            if (!collisionInfo) break;
            if (collisionInfo->thingyB && collisionInfo) {
                std::cout << ((Thingy*)collisionInfo->thingyB)->getName() << std::endl;

            }
            

            // find the best angle 
            glm::vec3 collisionDir = collisionInfo->pointA - playerTransform->getPosition();
            bool validPlatform = false;
            float normalAngle = glm::acos(glm::dot(up, collisionInfo->normalOnB));
            float directionAngle = glm::acos(glm::dot(-up, collisionDir));
            float platformAngle = (normalAngle + directionAngle)/2;
                // std::max(normalAngle, directionAngle);
            
            std::cout << platformAngle << std::endl;
            if (platformAngle < bestIncline && collisionInfo->thingyA) {
                bestIncline = platformAngle;
                Thingy* thingyA = (Thingy*)collisionInfo->thingyA;
                Thingy* thingyB = (Thingy*)collisionInfo->thingyB;

                if (thingyA && thingyA != host) {
                    platform = (Thingy*)collisionInfo->thingyA;
                } else if (thingyB && thingyB != host) {
                    platform = (Thingy*)collisionInfo->thingyB;
                }
            }
        }
        
        // if platform was found, we're touching the ground, proceed to jump
        if (platform) {
            RigidBody *platformRigidBody = &platform->getComponent<RigidBody>();
            Transform *platformTransform = &platform->getComponent<Transform>();
            
            // TODO: - additional check that velocity difference between platform and player isn't too great, or just factor that in when applying force
            
            // apply force to the thing we jumped off of
            if (platformRigidBody && platformTransform) {
                glm::vec3 forceOffset = playerTransform->getGlobalPosition() - platformTransform->getGlobalPosition();
                float platformMass = platformRigidBody->getMass();
                float playerMass = playerRigidBody->getMass();
                // if either mass is 0, the object is static. we'll just pretend its 1 as to not divide by 0
                float forceRatio;
                if (platformMass == 0) forceRatio = 1;
                else forceRatio = playerMass / platformMass / (playerMass + platformMass);

                platformRigidBody->addForce( (1-forceRatio) * -jumpStrength * up, forceOffset);
            force += up * forceRatio * jumpStrength;
                
            } else {
                // platform doesnt have required components? that's okay, just apply default force - as if the platform is just a static object
                force += up * jumpStrength;
            }
            jumpTimer = 200;
        }
    } else if (jumpTimer > 0) 
        jumpTimer-= physicsComponent->deltaTime;

    if (force.x || force.y || force.z) {
        playerRigidBody->addForce(force);
    }

    playerRigidBody->setAngularVelocity(glm::vec3(0, 0, 0));

    // do camera transform = lookat()
}


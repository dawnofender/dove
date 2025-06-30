#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"
#include "../thingy/panel/windowThingy.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>


CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(
    std::string && initialValue, 
    Physics* p, 
    Thingy* h, 
    Thingy* c, 
    RigidBody* r, 
    Transform* t, 
    float s, 
    float j, 
    float m, 
    float ms
) 
    : UpdatableComponent(std::move(initialValue)), 
    physicsComponent(p), 
    host(h), 
    camera(c), 
    playerRigidBody(r), 
    cameraTransform(t), 
    walkSpeed(s), 
    jumpStrength(j), 
    maxIncline(m), 
    mouseSensitivity(ms) 
{}

void PlayerController::serialize(Archive& archive) {
    Component::serialize(archive);
    archive &
        host &
        camera &
        cameraTransform &
        physicsComponent &
        playerRigidBody &
        jumpTimer &
        jumpCooldown &
        pitch &
        yaw &
        mouseSensitivity &
        walkSpeed &
        jumpStrength &
        maxIncline;
}


void PlayerController::update() {
    
    GLFWwindow* window = &Window::getActiveWindow().getGLFWwindow();
    if (!window) {
        std::cerr << value << ": window not found" << std::endl;
        return;
    }

    
    if (!cameraTransform) {
        if (!camera) {
            std::cerr << value << ": camera not found" << std::endl;
            return;
        }
        if (!(cameraTransform = &camera->getComponent<Transform>())) {
            // this one can be handled easily so no need to error 
            cameraTransform = &camera->addComponent<Transform>("Transform", camera);
        }
        return;
    }


    
    // # Mouse movement -> turn head
    // TODO: Move handle this input, especially reseting cursorpos, in a separate component
    // Get mouse movement vector
    glm::vec2 mouseMovement; 
    {
        double mousePosX, mousePosY;
        int width, height;
	    glfwGetCursorPos(window, &mousePosX, &mousePosY);
        glfwGetWindowSize(window, &width, &height);

        mouseMovement = glm::vec2(
            mouseSensitivity * float(width/2 - mousePosX),
            mouseSensitivity * float(height/2 - mousePosY)
        );

	    // Reset mouse position before next frame
	    glfwSetCursorPos(window, width/2, height/2);
    }

    
    // getting forward, up, and right vectors needed for head turning and motion
    glm::mat4 cameraMatrix = cameraTransform->getMatrix();
    glm::mat4 viewMatrix = glm::inverse(cameraMatrix);

    glm::vec3 up(0, 1, 0);
    glm::vec3 forward = glm::normalize(glm::vec3(viewMatrix[2])) * glm::vec3(1, 1, -1);
    // glm::vec3 right = glm::normalize(glm::vec3(viewMatrix[0]));
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    // when we get the forward vector, we must flip the z axis because in opengl, z is backwards
    
    yaw += mouseMovement.x;
    pitch += mouseMovement.y;
    
    const float piOver2 = 3.141592653589793f / 2.f;
    pitch = std::min( std::max( -piOver2, pitch ), piOver2 );
    
    cameraTransform->setOrientation(glm::quat_cast(glm::mat4(1)));
    cameraTransform->rotate(yaw, up);
    cameraTransform->rotate(pitch, glm::vec3(1, 0, 0));

    // old way of handling direction with lookat:
    //
    // glm::vec3 direction(
    //     cos(pitch) * sin(yaw),
    //     sin(pitch),
    //     cos(pitch) * cos(yaw)
    // );
    // 
    // glm::vec3 cameraPosition = cameraTransform->getPosition();
    // glm::vec3 target = cameraPosition + direction;
    //
    // cameraTransform->setMatrix(
    //     glm::inverse(glm::lookAt(
    //         cameraPosition,
    //         target,
    //         up
    // )));


    // handle locomotion
    
    if (!physicsComponent) return;
    if (!playerRigidBody) {
        if (!host) return;
        playerRigidBody = &host->getComponent<RigidBody>();
        return;
    }

    Transform *playerTransform  = &host->getComponent<Transform>();

    
    
    // this just keeps the player thingy upright. 
    // later, this could be replaced with a position lock constraint, or even applying force to accomplish this.
    glm::vec3 playerPosition = playerTransform->getPosition();
    playerTransform->setMatrix(
        glm::inverse(glm::lookAt(
            playerPosition,
            playerPosition + glm::vec3(0, 0, -1),
            up
    )));
    

    
    // get direction to move in from key inputs [W, A, S, D]
    glm::vec3 input = {0, 0, 0};
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS)
        input += forward;
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
        input -= forward;
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS) 
        input += right;
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
        input -= right;

    // normalize causes NAN if the length is zero, lets avoid this
    if (glm::length2(input) > 0) {
        // flatten and normalize input dir (for no flying movement)
        input = glm::normalize(input * glm::vec3(1, 0, 1));
    }
    
    // create a force in the input direction
    glm::vec3 force = walkSpeed * input;

    
    
    // jump logic
    
    // reset bounciness (it may have been changed to 1 when jumping off another rigidbody)

    if (
        // are we pressing space?
        glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS
        // has it been a bit since the last jump? 
        // jumpTimer >= jumpCooldown
    ) {
        // are we grounded? 
        // whats the ground? 
        auto collisions = physicsComponent->getCollisionInfo(host);
        Thingy* platform = nullptr;
        
        // the player's position will be needed to calculate angles and apply force.
        // Idealy, this would be handled differently - we could use the center of mass instead.

        // check each collision and compare to find a valid incline
        float bestIncline = maxIncline;
        glm::vec3 playerCenterOfMass = playerRigidBody->getCenterOfMass();
        for (auto&& collisionInfo : collisions) {
            if (!collisionInfo) break;
            bool validPlatform;

            // find the collision with the smallest incline.
            // the incline can be the normal of the surface we collided with.
            float normalAngle = glm::asin(glm::dot(up, collisionInfo->normalOnB));
            
            // However, this alone would create unexpected behavior when touching a ledge.
            // so, we also check which direction the collision occured in. 
            glm::vec3 collisionDir = glm::normalize(playerCenterOfMass - collisionInfo->pointA);
            float directionAngle = glm::acos(glm::dot(up, collisionDir));

            float platformIncline = std::max(normalAngle, directionAngle);
            
            // debugging prints:

            // std::cout << 
            //     "surface normal: " << normalAngle       << std::endl <<
            //     "direction:      " << directionAngle    << std::endl <<
            //     "incline:        " << platformIncline   << std::endl <<
            // std::endl;
            
            // save the best incline, we'll jump off that platform
            if (platformIncline < bestIncline && collisionInfo->thingyB) {
                bestIncline = platformIncline;
                platform = (Thingy*)collisionInfo->thingyB;
            }
        }
        
        
        // if platform was found, and it's at a reasonable incline, we're touching the ground. 
        bool grounded = bestIncline <= maxIncline;
        
        if (platform != nullptr && grounded) {
            RigidBody *platformRigidBody = &platform->getComponent<RigidBody>();
            Transform *platformTransform = &platform->getComponent<Transform>();

            // but we still might not be grounded. what if the platform is moving away from us? 
            
            if (platformRigidBody) {
                // get the difference between our velocity and the platform's:
                
                glm::vec3 platformVelocity = platformRigidBody->getLinearVelocity();
                glm::vec3 playerVelocity = playerRigidBody->getLinearVelocity();
                glm::vec3 relativeVelocity = platformVelocity - playerVelocity;
                // std::cout << "player velocity: " << glm::length2(playerVelocity) << std::endl;
                // std::cout << "platfm velocity: " << glm::length2(platformVelocity) << std::endl;
                
                // if that difference points up, the platform and player are moving towards each other on the up axis.
                // therefor we are grounded.

                // we can use dot product to check this.
                // dot(relativeVelocity, up) is positive if grouneded and negative if not.

                // std::cout << "dot velocity: " << glm::dot(relativeVelocity, up) << std::endl;
                // std::cout << "velocity amnt: " << glm::length2(relativeVelocity) << std::endl;
                grounded = (
                    glm::dot(relativeVelocity, up) >= 0 ||
                    // for some reason, the player's velocity is always just a really small number, so now we just check if it's close to zero
                    glm::length2(relativeVelocity) <= 0.01f 
                );

                if (grounded) {
                    glm::vec3 platformCenterOfMass = platformRigidBody->getCenterOfMass();
                    glm::vec3 forceOffset = playerCenterOfMass - platformCenterOfMass;

                    // I really want to just bounce the player off the ground but the physics engine isn't letting me ):
                    // playerRigidBody->setBounciness(1);
                    
                    // so instead we're doing our own calculations for semi-realistic physics:
                    float platformMass = platformRigidBody->getMass();
                    float playerMass = playerRigidBody->getMass();
                    // if either mass is 0, the object is static. we'll just pretend its 1 as to not divide by 0
                    float forceRatio;
                    if (platformMass == 0) forceRatio = 1;
                    else forceRatio = playerMass / (playerMass + platformMass);
                    
                    
                    force += jumpStrength * up;
                    platformRigidBody->addForce( (1-forceRatio) * jumpStrength * -up, forceOffset);
                }

            } else {
                force += up * jumpStrength;
                // platform doesnt have necessary components? that's okay, just apply default force - as if the platform is a static object
            }
            
            // std::cout << "grounded: " << grounded << std::endl;


            jumpTimer = 0;
            // std::cout << "jump force: " << force.y << std::endl;
        }

    } else {
        jumpTimer += physicsComponent->deltaTime;
        // playerRigidBody->setBounciness(0);
    }
    
    playerRigidBody->addForce(force, glm::vec3(0));

    // playerRigidBody->setAngularVelocity(glm::vec3(0, 0, 0));

    // do camera transform = lookat()
}


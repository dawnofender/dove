#include "playerControllerComponent.hpp"
#include "rendering/cameraComponent.hpp"
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
    : UpdatableComponent(
    std::move(initialValue)), 
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

    Transform *playerTransform  = &host->getComponent<Transform>();

    // # Mouse movement -> turn head
    // TODO: Move handle this input, especially reseting cursorpos, in a separate component
    
    // Get mouse movement vector
    glm::vec2 mouseMovement; 
    {
        double mousePosX, mousePosY;
        int width, height;
	    glfwGetCursorPos(Camera::activeWindow, &mousePosX, &mousePosY);
        glfwGetWindowSize(Camera::activeWindow, &width, &height);

        mouseMovement = glm::vec2(
            mouseSensitivity * float(width/2 - mousePosX),
            mouseSensitivity * float(height/2 - mousePosY)
        );

	    // Reset mouse position before next frame
	    glfwSetCursorPos(Camera::activeWindow, width/2, height/2);
    }

    
    // getting forward, up, and right vectors needed for head turning and motion
    glm::mat4 cameraMatrix = cameraTransform->getMatrix();
    glm::mat4 viewMatrix = glm::inverse(cameraMatrix);

    glm::vec3 up(0, 1, 0);
    glm::vec3 forward = glm::normalize(glm::vec3(viewMatrix[2])) * glm::vec3(1, 1, -1);
    // glm::vec3 right = glm::normalize(glm::vec3(viewMatrix[0]));
    glm::vec3 right = glm::cross(forward, up);
    // when we get the forward vector, we must flip the z axis because in opengl, z is backwards
    
    yaw += mouseMovement.x;
    pitch += mouseMovement.y;
    
    const float halfPi = 3.14159265358978f / 2.f;
    pitch = std::min( std::max( -halfPi, pitch ), halfPi );
    
    glm::vec3 direction(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    );
    
    glm::vec3 cameraPosition = cameraTransform->getPosition();
    glm::vec3 target = cameraPosition + direction;

    cameraTransform->setMatrix(
        glm::inverse(glm::lookAt(
            cameraPosition,
            target,
            up
    )));
    
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
    
    // create a force in the input direction
    glm::vec3 force = walkSpeed * input;

    
    
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
        Thingy* platform = nullptr;
        
        // the player's position will be needed to calculate angles and apply force.
        // Idealy, this would be handled differently - we could use the center of mass instead.

        // check each collision and compare to find a valid incline
        float bestIncline = maxIncline;
        for (auto&& collisionInfo : collisions) {
            if (!collisionInfo) break;
            if (collisionInfo->thingyB && collisionInfo) {
                std::cout << ((Thingy*)collisionInfo->thingyB)->getName() << std::endl;

            }
            

            // find the best angle 
            glm::vec3 collisionDir = glm::normalize(collisionInfo->pointB - collisionInfo->pointA);
            float directionAngle = glm::asin(glm::dot(up, collisionDir));
            float normalAngle = glm::asin(glm::dot(up, collisionInfo->normalOnB));
            float platformIncline = std::max(normalAngle, directionAngle);
            bool validPlatform;

            std::cout << 
                "surface normal: " << normalAngle       << std::endl <<
                "direction:      " << directionAngle    << std::endl <<
                "incline:        " << platformIncline   << std::endl <<
            std::endl;

            if (platformIncline < bestIncline && collisionInfo->thingyA) {
                bestIncline = platformIncline;
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
        if (platform != nullptr) {
            RigidBody *platformRigidBody = &platform->getComponent<RigidBody>();
            Transform *platformTransform = &platform->getComponent<Transform>();
            
            // TODO: - additional check that velocity difference between platform and player isn't too great, or just factor that in when applying force
            
            // apply force to the thing we jumped off of
            if (platformRigidBody && platformTransform) {
                // FIX: use center of mass instead
                glm::vec3 forceOffset = playerPosition - platformTransform->getGlobalPosition();
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


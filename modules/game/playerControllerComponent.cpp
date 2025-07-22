#include "playerControllerComponent.hpp"

#include <core/math.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GLFW/glfw3.h>

#include "../camera/cameraComponent.hpp"
#include "../window/window.hpp"
#include "../rendering/modelRendererComponent.hpp"
#include "../physics/boxColliderComponent.hpp"


CLASS_DEFINITION(Component, PlayerController)


PlayerController::PlayerController(
    std::string && initialName, 
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
    : UpdatableComponent(std::move(initialName)), 
    physicsComponent(p), 
    host(h), 
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
    
    // TODO: get window from camera instead of using active window
    Window &window = Window::getActiveWindow();
    GLFWwindow *glfwWindow = &window.getGLFWwindow();
    
    if (!cameraTransform) {
        std::cerr << name << ": camera transform not found" << std::endl;
        return;
    }

    if (!playerRigidBody) {
        std::cerr << name << ": rigidbody not found" << std::endl;
        if (!host) return;
        playerRigidBody = &host->getComponent<RigidBody>();
        return;
    }

    if (!physicsComponent) {
        physicsComponent = playerRigidBody->getPhysicsWorld();
    }

    Dove::Vector2 mouseMovement = calculateMouseMovement(window);

    
    // getting forward, up, and right vectors needed for head turning and motion
    Dove::Matrix4 cameraMatrix = cameraTransform->getMatrix();
    Dove::Matrix4 viewMatrix = glm::inverse(cameraMatrix);

    Dove::Vector3 up(0, 1, 0);
    // when we get the forward vector, we must flip the z axis because in opengl, z is backwards
    Dove::Vector3 forward = glm::normalize(Dove::Vector3(viewMatrix[2])) * Dove::Vector3(1, 1, -1);
    Dove::Vector3 right = glm::normalize(glm::cross(forward, up));
    
    yaw += mouseMovement.x;
    pitch += mouseMovement.y;
    
    const float piOver2 = 3.141592653589793f / 2.f;
    pitch = std::min( std::max( -piOver2, pitch ), piOver2 );
    
    cameraTransform->setOrientation(glm::quat_cast(Dove::Matrix4(1)));
    cameraTransform->rotate(yaw, up);
    cameraTransform->rotate(pitch, Dove::Vector3(1, 0, 0));


    // handle locomotion
    Transform *playerTransform  = &host->getComponent<Transform>();

    
    
    // this just keeps the player thingy upright. 
    // later, this could be replaced with a position lock constraint, or even applying force to accomplish this.
    Dove::Vector3 playerPosition = playerTransform->getPosition();
    playerTransform->setMatrix(
        glm::inverse(glm::lookAt(
            playerPosition,
            playerPosition + Dove::Vector3(0, 0, -1),
            up
    )));
    

    
    // get direction to move in from key inputs [W, A, S, D]
    Dove::Vector3 input = {0, 0, 0};
    if (glfwGetKey( glfwWindow, GLFW_KEY_W ) == GLFW_PRESS)
        input += forward;
    if (glfwGetKey( glfwWindow, GLFW_KEY_S ) == GLFW_PRESS)
        input -= forward;
    if (glfwGetKey( glfwWindow, GLFW_KEY_D ) == GLFW_PRESS) 
        input += right;
    if (glfwGetKey( glfwWindow, GLFW_KEY_A ) == GLFW_PRESS)
        input -= right;

    // normalize causes NAN if the length is zero, lets avoid this
    if (glm::length2(input) > 0) {
        // flatten and normalize input dir (for no flying movement)
        input = glm::normalize(input * Dove::Vector3(1, 0, 1));
    }
    
    // create a force in the input direction
    Dove::Vector3 force = walkSpeed * input;

    
    
    // jump logic
    
    // reset bounciness (it may have been changed to 1 when jumping off another rigidbody)

    if (
        // are we pressing space?
        glfwGetKey( glfwWindow, GLFW_KEY_SPACE ) == GLFW_PRESS
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
        Dove::Vector3 playerCenterOfMass = playerRigidBody->getCenterOfMass();
        for (auto&& collisionInfo : collisions) {
            if (!collisionInfo) break;
            bool validPlatform;

            // find the collision with the smallest incline.
            // the incline can be the normal of the surface we collided with.
            float normalAngle = glm::asin(glm::dot(up, collisionInfo->normalOnB));
            
            // However, this alone would create unexpected behavior when touching a ledge.
            // so, we also check which direction the collision occured in. 
            Dove::Vector3 collisionDir = glm::normalize(playerCenterOfMass - collisionInfo->pointA);
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
                
                Dove::Vector3 platformVelocity = platformRigidBody->getLinearVelocity();
                Dove::Vector3 playerVelocity = playerRigidBody->getLinearVelocity();
                Dove::Vector3 relativeVelocity = platformVelocity - playerVelocity;
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
                    Dove::Vector3 platformCenterOfMass = platformRigidBody->getCenterOfMass();
                    Dove::Vector3 forceOffset = playerCenterOfMass - platformCenterOfMass;

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
    
    playerRigidBody->addForce(force, Dove::Vector3(0));

    
    // TODO:  
    // get rid of this stuff, its just for testing:
    {
        // on click, make a cube
        if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT)) {
            // Thingy *root = &host->getRoot();
            // std::cout << "test" << std::endl;
            // Thingy *testCube3 = &root->addChild("Cube");
            // Transform *testCube3Transform = &testCube3->addComponent<Transform>("Transform", testCube3);
            // testCube3Transform->setPosition({0, 16.f, 0});
            // testCube3->addComponent<BoxCollider>("BoxCollider", Dove::Vector3(.5f, .5f, .5f));
            // testCube3->addComponent<RigidBody>("RigidBody", physicsComponent, testCube3, 1.f);
            // testCube3->addComponent<ModelRenderer>("ModelRenderer", testCube3, testMaterial, cube);
        }
            
        // if we fall into the void, go back to spawn
        if (playerTransform->getGlobalPosition().y < -128) {
            playerTransform->setGlobalMatrix(Dove::Matrix4(1));
        }
    }
    
}

Dove::Vector2 PlayerController::calculateMouseMovement(Window& window) const {
    GLFWwindow* glfwWindow = &window.getGLFWwindow();

    // # Mouse movement -> turn head
    // TODO: Move this, especially reseting cursorpos, to a separate component
    // Get mouse movement vector
    Dove::Vector2 mouseMovement; 
    
    double mousePosX, mousePosY;
    int width, height;
	glfwGetCursorPos(glfwWindow, &mousePosX, &mousePosY);
	// std::cout << mousePosX << ", " << mousePosY << std::endl;
    glfwGetWindowSize(glfwWindow, &width, &height);

    mouseMovement = Dove::Vector2(
        mouseSensitivity * float(width/2 - mousePosX),
        mouseSensitivity * float(height/2 - mousePosY)
    );

	// Reset mouse position before next frame
	glfwSetCursorPos(glfwWindow, width/2, height/2);

    return mouseMovement;
}

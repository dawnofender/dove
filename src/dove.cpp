#define STB_IMAGE_IMPLEMENTATION

#include <lib/setup.hpp>

#include "defaultAssets.hpp"
#include "test_dove.hpp"

#include "components/transformComponent.hpp"
#include "components/playerControllerComponent.hpp"
#include "components/physics/physicsComponent.hpp"
#include "components/physics/sphereColliderComponent.hpp"
#include "components/physics/boxColliderComponent.hpp"
#include "components/physics/rigidBodyComponent.hpp"
#include "components/rendering/objectRendererComponent.hpp"
#include "components/rendering/skyRendererComponent.hpp"
#include "components/rendering/cameraComponent.hpp"

#include <glm/gtx/io.hpp>


int main() {
    std::cout << "hi!" << std::endl;

    // #########
    // # tests #
    // #########
    
    // defined in test_dove.hpp
    std::cout << "Running tests... " << std::endl;
    runTests(); 
    std::cout << "Running tests: done" << std::endl;
    

    // #########
    // # setup #
    // #########

    std::cout << std::endl << "Press enter to start." << std::endl;
    std::cin.get();

    std::cout << "Initializing Dove..." << std::endl;
    
    if (!initDove()) {
        std::cerr << "Failed to initialize Dove" << std::endl;
        return -1;
    }
    
    // Essential thingies
    Window* window = new Window("main window");
    
    // Creates a basic cube model, some materials, textures, and shaders
    loadDefaultAssets();





    // ##################
    // # world creation #
    // ##################
    
    std::cout << "Building world..." << std::endl;
    
    // the universe
    Thingy *universe = new Thingy("universe");
    Physics *physics = &universe->addComponent<Physics>("Laws Of Physics");
    
    // the player
    Thingy *player = &universe->addChild("it's you!");
    Transform *playerTransform = &player->addComponent<Transform>("Transform", player);
    player->addComponent<SphereCollider>("SphereCollider", 0.5f);
    RigidBody *playerRigidBody = &player->addComponent<RigidBody>("RigidBody", physics, player, 5.f, false, false);
    
    playerRigidBody->setFriction(1.5f);
    playerRigidBody->setRollingFriction(100.f);
    playerRigidBody->setSpinningFriction(0.9f);

    playerTransform->setPosition(glm::vec3(0, 0, -2)); // just moving back a bit so the boxes dont crush us
    
    // main camera
    Thingy *camera = &player->addChild("Perception");
    Transform *perceptionTransform = &camera->addComponent<Transform>("Transform", camera);
    camera->addComponent<Camera>("Camera", camera, window);
    
    player->addComponent<PlayerController>("PlayerController", physics, player, camera, playerRigidBody, perceptionTransform, 100);

    // # Basic scene
    // environment
    Thingy *sky = &universe->addChild("Sky");
    sky->addComponent<SkyRenderer>("SkyRenderer", testSkyMaterial, cube);
   
    // ground
    Thingy *ground = &universe->addChild("Ground");
    Transform *groundTransform = &ground->addComponent<Transform>("Transform", ground);
    groundTransform->setPosition({0, -16.f, 0});
    groundTransform->setScale({32.f, 32.f, 32.f});
    ground->addComponent<ObjectRenderer>("ObjectRenderer", ground, testMaterial3, cube);
    ground->addComponent<BoxCollider>("BoxCollider", ground, glm::vec3(16.f, 16.f, 16.f));
    RigidBody *groundRigidBody = &ground->addComponent<RigidBody>("RigidBody", physics, ground, 0.f, false, true);
    groundRigidBody->setFriction(0.9f);
    
    // cubes
    Thingy *testCube = &universe->addChild("Cube");
    Transform *cubeTransform = &testCube->addComponent<Transform>("Transform", testCube);
    cubeTransform->setPosition({0, 16.f, 0});
    testCube->addComponent<BoxCollider>("BoxCollider", testCube, glm::vec3(.5f, .5f, .5f));
    testCube->addComponent<RigidBody>("RigidBody", physics, testCube, 10.f);
    testCube->addComponent<ObjectRenderer>("ObjectRenderer", testCube, testMaterial, cube);
    
    // TODO: thingy copy constructor
    Thingy *testCube2 = &universe->addChild("Cube");
    Transform *cubeTransform2 = &testCube2->addComponent<Transform>("Transform", testCube2);
    cubeTransform2->setPosition({0, 16.f, 0});
    testCube2->addComponent<BoxCollider>("BoxCollider", testCube2, glm::vec3(.5f, .5f, .5f));
    testCube2->addComponent<RigidBody>("RigidBody", physics, testCube2, 1.f);
    testCube2->addComponent<ObjectRenderer>("ObjectRenderer", testCube2, testMaterial2, cube);
    
    std::cout << "Building world: done" << std::endl;

    // #############
    // # main loop #
    // #############
    
    // TODO: any functionality written directly into this loop will later be moved into its own class. 
    //  - fps tracking
    //  - input (mouse pos, keyboard, click raycasting)
    //  - this list used to be way longer :)

    double lastFrameTime = glfwGetTime();
    int nbFrames = 0;

		
    double lastTime;
    double time;
    
    // just making sure the FPS print doesn't write over anything
    std::cout << std::endl << std::endl;
    do {
        // #######
        // # fps #
        // #######
        lastTime = time;
        time = glfwGetTime();
        nbFrames++;
        if ( time - lastFrameTime >= 1.0) { // If last cout was more than 1sec ago
            // print current framerate
            // while we're here, might as well print the player's location
            std::cout << 
                "\033[2A" << // move up two lines to overwrite 
                "fps: " << nbFrames << std::endl <<
                "position: " << playerTransform->getPosition() << 
            std::endl;
            

            nbFrames = 0;
            lastFrameTime += 1.0;
        } 


        
        // ##############
        // # world loop #
        // ##############
        
        // TODO: move click-raycasting into player controller component
        
        // handle raycasting for player clicking on objects 
        double mouseX, mouseY;

	    glfwGetCursorPos(&window->getGLFWwindow(), &mouseX, &mouseY);
        glm::vec4 lRayStart_NDC(
            (mouseX - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
        	  (mouseY - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
        	  -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
        	  1.0f
        );

        glm::vec4 lRayEnd_NDC(
        	  (mouseX - 0.5f) * 2.0f,
        	  (mouseY - 0.5f) * 2.0f,
        	  0.0,
        	  1.0f
        );
        
        glm::mat4 projectionMatrix = Camera::getProjectionMatrix();
        glm::mat4 viewMatrix = Camera::getViewMatrix();
        glm::mat4 M = glm::inverse(projectionMatrix * viewMatrix);
        glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);
        
        auto rayCastInfo = physics->rayCast(playerTransform->getPosition(), lRayDir_world, 1000);
        Thingy* hoverObject = rayCastInfo.thingy;
        
        // on click, do something to hovered object 
        if (glfwGetMouseButton(&window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) && hoverObject) {
            Thingy *testCube = &universe->addChild("Cube");
            Transform *cubeTransform = &testCube->addComponent<Transform>("Transform", testCube);
            cubeTransform->setPosition({0, 16.f, 0});
            testCube->addComponent<BoxCollider>("BoxCollider", testCube, glm::vec3(.5f, .5f, .5f));
            // testCube->addComponent<SphereCollider>("SphereCollider", 0.5f);
            testCube->addComponent<RigidBody>("RigidBody", physics, testCube, 1.f);
            testCube->addComponent<ObjectRenderer>("ObjectRenderer", testCube, testMaterial, cube);
        }
        
        // if we fall into the void, go back to spawn
        if (playerTransform->getGlobalPosition().y < -128) {
            playerTransform->setGlobalMatrix(glm::mat4(1));
        }

        
        // # physics - should be one function in physics component
        Physics::simulateAll();

        // # updating components
        UpdatableComponent::updateAll();

        // # rendering stuff
        Camera::renderAll();

    } while (glfwGetKey(&window->getGLFWwindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(&window->getGLFWwindow()) == 0);
    // } while (glfwWindowShouldClose(window) == 0);

    std::cout << "cleaning up..." << std::endl;
    delete universe;
    doveTerminate();

    std::cout << "bye bye!" << std::endl;
    return 0;
}






    // #########
    // # other #
    // #########

    // # GAIA
    
    // Thingy *gaia = &universe->addChild("gaia");
    // gaia->addComponent<Gaia>("Gaia", gaia, player);

    // put player on serface (temporary)
    // playerController->teleport({0, 4210, 0});

    // auto *world = &gaia->getComponent<Gaia>();
    // Octree cellTree(0, 24);
    // world->createWorld(&cellTree);
    // 23 - a bit bigger than earth
    // 65 - max before math breaks at edges (currently breaks world entirely, but
    // can be fixed by generating from the center instead of the corner) 90 -
    // bigger than the observable universe
    
    // world->startGeneratingWorld();
    

#include <iostream>
#include <memory>

#include "../core/component/updatableComponent.hpp"
// #include "../event/momento.hpp"
#include "../command/command.hpp"
#include "../3d/transformComponent.hpp"
#include "../game/playerControllerComponent.hpp"
#include "../physics/physicsComponent.hpp"
#include "../physics/sphereColliderComponent.hpp"
#include "../physics/boxColliderComponent.hpp"
#include "../physics/rigidBodyComponent.hpp"
#include "../rendering/modelRendererComponent.hpp"
#include "../rendering/skyRendererComponent.hpp"
#include "../camera/cameraComponent.hpp"
#include "../window/window.hpp"
#include "../link/link.hpp"
#include "../rendering/opengl.cpp"

#include "defaultAssets.hpp"


extern "C" void buildTestDream() {
    setupOpenGL();

    std::shared_ptr<Thingy> dove = std::make_shared<Thingy>("dove");
    dove->init();
    
    // Essential things
    Window *window = &dove->addChild<Window>("main window");
    
    // Event *timeline = new Event(dove);
    
    // TEMPORARY: Creates a basic cube model, some materials, imports some textures and shaders
    loadDefaultAssets();


    // ##################
    // # world creation #
    // ##################
    
    std::cout << "Building world..." << std::endl;
    
    // the universe
    Thingy *universe = &dove->addChild("universe");
    
    // Thingy *universe = new Thingy("universe");
    // required for shared_from_this to work inside universe object while creating children
    // ideally, once this system is handled by commands, all root thingies will have shared ptrs to them as part of a root object, or static inside the thingy class. 
    // this way you can travel between them, or see them all on a list or whatever
    // std::unique_ptr<Thingy> universePtr(universe);
    
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
    ground->addComponent<ModelRenderer>("ModelRenderer", ground, testMaterial3, cube);
    ground->addComponent<BoxCollider>("BoxCollider", glm::vec3(16.f, 16.f, 16.f));
    RigidBody *groundRigidBody = &ground->addComponent<RigidBody>("RigidBody", physics, ground, 0.f, false, true);
    groundRigidBody->setFriction(0.9f);
    
    // cubes
    Thingy *testCube = &universe->addChild("Cube");
    Transform *cubeTransform = &testCube->addComponent<Transform>("Transform", testCube);
    cubeTransform->setPosition({0, 16.f, 0});
    testCube->addComponent<BoxCollider>("BoxCollider", glm::vec3(.5f, .5f, .5f));
    testCube->addComponent<RigidBody>("RigidBody", physics, testCube, 10.f);
    testCube->addComponent<ModelRenderer>("ModelRenderer", testCube, testMaterial, cube);
    
    // TODO: thingy copy constructor
    Thingy *testCube2 = &universe->addChild("Cube");
    Transform *cubeTransform2 = &testCube2->addComponent<Transform>("Transform", testCube2);
    cubeTransform2->setPosition({0, 16.f, 0});
    testCube2->addComponent<BoxCollider>("BoxCollider", glm::vec3(.5f, .5f, .5f));
    testCube2->addComponent<RigidBody>("RigidBody", physics, testCube2, 1.f);
    testCube2->addComponent<ModelRenderer>("ModelRenderer", testCube2, testMaterial2, cube);
    
    std::cout << "Building world: done" << std::endl;

    // Link *link = new Link(player);
    // link->start();

    // #############
    // # main loop #
    // #############
    
    // TODO: any functionality written directly into this loop will later be moved into its own class. 
    //  - fps tracking
    //  - input (mouse pos, keyboard, click raycasting)
    //  - this list used to be way longer :)
    
    Window* activeWindow = &Window::getActiveWindow();
    // Window* activeWindow = window;
    
    int frame = 0;
    double time;
    double lastTime;
    double lastFrameTime = glfwGetTime();
    int nbFrames = 0;
    int framerate = 0;
    
    // std::shared_ptr<Thingy> universePtr = universe->new_shared_from_this();
    
    // just making sure the FPS print doesn't write over anything
    std::cout << std::endl << std::endl;
    do {
    
        
        // ##############
        // # world loop #
        // ##############
        
    
        // # testing events
        // momento
        // if (frame == 500) {
        //     Momento *momento = new Momento(universePtr);
        //
        //     momento->invoke();
        //     // dove->removeChildren<Window>();
        //     momento->restore();
        //     
        // }
        //
        // // commands
        // if (frame % 1000 == 999) {
        //     std::string commandInput = "";
        //     std::cin >> commandInput;
        //     
        // }
    
    
        // # important stuff
        // later, these functions will actually be handled by some component on the world.
        // each world would have its own event loop, and would activate other components in a specific order.
        // eg. activate physics manager component, update updatablecomponent manager component, update cameramanager component.
        // all of these are derived from some component manager base class
        // orrr, one component handles all of that, and any kind of component can register itself to be updated with a specified priority
        // that might be better

        // tick physics
        // std::cout << "physics" << std::endl;
        Physics::simulateAll();
    
        // update components
        // std::cout << "component updates" << std::endl;
        UpdatableComponent::updateAll();
    
        // render everything
        // std::cout << "rendering" << std::endl;
        Camera::renderAll();
    
        // printThingyTree(universe.get());
    
    
        // #######
        // # fps #
        // #######
        
        lastTime = time;
        time = glfwGetTime();
        nbFrames++;
        frame++;
    
        if ( time - lastFrameTime >= 1.0) { // If last cout was more than 1sec ago
            framerate = nbFrames;
            nbFrames = 0;
            lastFrameTime += 1.0;
            // // print current framerate
            // // while we're here, might as well print the player's location
            // std::cout << 
            //     "fps: " << framerate << std::endl <<
            //     "position: " << playerTransform->getPosition() << 
            //     "\033[2A" << // move up two lines to overwrite 
            // std::endl;
        }
    
        activeWindow = &Window::getActiveWindow();
    } while (glfwGetKey(&activeWindow->getGLFWwindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(&activeWindow->getGLFWwindow()) == 0);
    // } while (glfwWindowShouldClose(window) == 0);

    std::cout << std::endl << "cleaning up..." << std::endl;
    dove.reset();
    
    // TODO: move back into doveTerminate() function, whenever we figure out where to put that
    glfwTerminate();
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

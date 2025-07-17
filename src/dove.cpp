#define STB_IMAGE_IMPLEMENTATION

#include <lib/setup.hpp>

#include "defaultAssets.hpp"
#include "test_dove.hpp"

#include "thingy/panel/windowThingy.hpp"
#include "event/momento.hpp"
#include "command/command.hpp"

#include "components/transformComponent.hpp"
#include "components/playerControllerComponent.hpp"
#include "components/physics/physicsComponent.hpp"
#include "components/physics/sphereColliderComponent.hpp"
#include "components/physics/boxColliderComponent.hpp"
#include "components/physics/rigidBodyComponent.hpp"
#include "components/rendering/modelRendererComponent.hpp"
#include "components/rendering/skyRendererComponent.hpp"
#include "components/rendering/cameraComponent.hpp"

#include <glm/gtx/io.hpp>

#include <assimp/Importer.hpp>

void printThingyTree(Thingy *thingy, int &depth) {

    for (int i = 0; i < depth; i++)
        std::cout << "| ";
    std::cout << thingy->getName() << " : " <<thingy->getType() << std::endl;

    for (auto && component : thingy->components) {
        for (int i = 0; i < depth; i++)
            std::cout << "| ";
        std::cout << "- " << component->name << " : " << component->getType() << std::endl;
    }
    
    depth++;
    for (auto && child : thingy->children) {
        printThingyTree(child.get(), depth);
    }
    depth--;
}

void printThingyTree(Thingy *thingy) {
    int depth = 0;
    printThingyTree(thingy, depth);
}

int main() {
    std::cout << "hi!" << std::endl;

    // #########
    // # tests #
    // #########
    
    // defined in test_dove.hpp
    // runTests(); 
    

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
    
    std::shared_ptr<Thingy> dove = std::make_shared<Thingy>("dove");
    dove->init();

    // Essential things
    Window *window = &dove->addChild<Window>("main window");
    
    Event *timeline = new Event(dove);
    
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

    // #############
    // # main loop #
    // #############
    
    // TODO: any functionality written directly into this loop will later be moved into its own class. 
    //  - fps tracking
    //  - input (mouse pos, keyboard, click raycasting)
    //  - this list used to be way longer :)
    
    Window* activeWindow = &Window::getActiveWindow();

    int frame = 0;
    double time;
    double lastTime;
    double lastFrameTime = glfwGetTime();
    int nbFrames = 0;
    int framerate = 0;
    

    std::shared_ptr<Thingy> universePtr = universe->new_shared_from_this();

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
        // these should actually be handled by some components on the world that track instances of components.
        // on initiation, certain components would be registered under the world so they can all be called at once - but only if that world is active.
        
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
    

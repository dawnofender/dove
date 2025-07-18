#include "link.hpp"

#include "../window/window.hpp"
#include "../physics/physicsComponent.hpp"
#include "../core/component/updatableComponent.hpp"

Link::Link(Thingy *p) 
    :player(p) {
    start();
}


void Link::start() {
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

}




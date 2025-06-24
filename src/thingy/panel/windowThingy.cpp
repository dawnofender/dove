#include "windowThingy.hpp"

CLASS_DEFINITION(Panel, Window)


Window::Window(std::string && initialName) 
    : Panel(std::move(initialName)) {
    
    glfwWindow = glfwCreateWindow(1024, 768, name.c_str(), NULL, NULL);

    if (glfwWindow == NULL) {
        std::cerr << "Failed to open GLFW window.\n" << std::endl;
        return;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents(); // try removing in a sec

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    activeWindow = this;
}

Window& Window::getActiveWindow() {
    return *activeWindow;
}

GLFWwindow& Window::getGLFWwindow() {
    return *glfwWindow;
}

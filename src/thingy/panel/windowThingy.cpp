#include "windowThingy.hpp"

CLASS_DEFINITION(Panel, Window)


Window::Window(std::string && initialName) 
    : Panel(std::move(initialName)) {}


Window::~Window() {
    if (activeWindow == this)
        activeWindow = nullptr;
    closeGLFWwindow(glfwWindow);
}

void Window::closeGLFWwindow(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void Window::init() {
    
    glfwWindow = glfwCreateWindow(1024, 768, name.c_str(), NULL, NULL);

    if (glfwWindow == NULL) {
        std::cerr << "ERROR: Window: Failed to open GLFW window." << std::endl;
        return;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents(); // try removing in a sec
    
    glfwSetWindowCloseCallback(glfwWindow, closeGLFWwindow);

    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: Window: Failed to initialize GLEW" << std::endl;
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

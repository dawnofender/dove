#include "setup.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool initDove() {
    if (!setupOpenGL()) {
        std::cerr << "Failed to setup OpenGL" << std::endl;
        return false;
    }


    return true;
}


bool setupOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // to make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_COMPAT_PROFILE); 
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  
    // glewExperimental = true;
    // glClearColor(0.6f, 0.7f, 0.9f, 0.0f);

    GLFWwindow *tempWindow = glfwCreateWindow(1024, 768, "dove", NULL, NULL);
    glfwMakeContextCurrent(tempWindow);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glfwDestroyWindow(tempWindow);

    return true;
}


void doveTerminate() {
    glfwTerminate();
}

#ifndef WINDOWTHINGY_HPP
#define WINDOWTHINGY_HPP

#include "panelThingy.hpp"
#include <GLFW/glfw3.h>

// TODO: 
// - support multiple windows open

class Window : public Panel {
CLASS_DECLARATION(Window)
public: 
    Window(std::string && initialName = "Window");
    static void setActiveWindow(Window *newActiveWindow);
    static Window& getActiveWindow();
    GLFWwindow& getGLFWwindow();
    
protected:
    GLFWwindow *glfwWindow;

private:
    static inline Window *activeWindow;
};

#endif

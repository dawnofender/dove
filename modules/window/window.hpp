#ifndef WINDOWTHINGY_HPP
#define WINDOWTHINGY_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../panel/panelThingy.hpp"


// TODO: 
// - support multiple windows open
// - map glfwWindow to Window object to close it better
// - maybe handle window serialization differently so we aren't closing and opening glfw windows, just moving existing objects based on some ID or something?

class Window : public Panel {
CLASS_DECLARATION(Window)
public: 
    Window(std::string && initialName = "Window");
    
    virtual ~Window();

    virtual void init() override;

    static void setActiveWindow(Window *newActiveWindow);
    static Window& getActiveWindow();

    GLFWwindow& getGLFWwindow();
    static void closeGLFWwindow(GLFWwindow* window);

    
protected:
    GLFWwindow *glfwWindow;

private:
    static inline Window *activeWindow;
};


#endif

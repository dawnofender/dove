#ifndef CAMERACOMPONENT_HPP
#define CAMERACOMPONENT_HPP

#include "../component.hpp"
#include "../transformComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/thingy/thingy.hpp>
#include <GLFW/glfw3.h>

// TODO: 
//  - replace GLFWwindow with a render target object (that can contain a window)
//  - move window and getActiveWindow into their own thingy-derived class
//    - make them part of the ui panel system? so panels can be drawn on windows outside of the world

class Camera : public Component {
CLASS_DECLARATION(Camera)
public:
    Camera(std::string &&initialValue = "Camera", Thingy* h = nullptr, GLFWwindow* w = nullptr, float fov = 90.f, int dw = 1024, int dh = 768, float n = .1f, float f = 1024.f);
    ~Camera();
    
    static GLFWwindow* getActiveWindow();
    static void setActiveWindow(GLFWwindow* newActiveWindow);
    static glm::mat4 getProjectionMatrix();
    static glm::mat4 getViewMatrix();
    static void renderAll();
    void render();

private: 
    Thingy* host;
    GLFWwindow* window = nullptr; //NOTE: windows should be handled somewhere else, maybe a 'render target' object would be better?
    static inline glm::mat4 projectionMatrix;
    static inline glm::mat4 viewMatrix;
	  float FoV;
    int width;
    int height;
    float near;
    float far;
    static inline GLFWwindow* activeWindow;

protected:
    static inline std::vector<Camera *> cameras;
};


#endif

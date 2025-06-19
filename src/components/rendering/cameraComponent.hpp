#ifndef CAMERACOMPONENT_HPP
#define CAMERACOMPONENT_HPP

#include "../component.hpp"
#include "../transformComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/thingy/thingy.hpp>
#include <GLFW/glfw3.h>

class Camera : public Component {
CLASS_DECLARATION(Camera)
public:
    Camera(std::string &&initialValue = "Camera", Thingy* h = nullptr, GLFWwindow* w = nullptr, float fov = 90.f, int dw = 1024, int dh = 768, float n = .1f, float f = 1024.f);
    ~Camera();
    static glm::mat4 getProjectionMatrix();
    static glm::mat4 getViewMatrix();
    static void renderAll();
    void see();

    static inline GLFWwindow* activeWindow = nullptr;
private: 
    Thingy* host;
    GLFWwindow* window; //NOTE: windows should be handled somewhere else, maybe a 'render target' object would be better?
    static inline glm::mat4 projectionMatrix;
    static inline glm::mat4 viewMatrix;
	  float FoV;
    int width;
    int height;
    float near;
    float far;

protected:
    static inline std::vector<Camera *> cameras;
};


#endif

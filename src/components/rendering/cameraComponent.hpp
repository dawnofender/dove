#ifndef CAMERACOMPONENT_HPP
#define CAMERACOMPONENT_HPP

#include "../component.hpp"
#include "../transformComponent.hpp"
#include "../../thingy/panel/windowThingy.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/thingy/thingy.hpp>
#include <set>

// TODO: 
//  - replace GLFWwindow with a render target object (that can contain a window)
//  - move window and getActiveWindow into their own thingy-derived class
//    - make them part of the ui panel system? so panels can be drawn on windows outside of the world

class Camera : public Component {
CLASS_DECLARATION(Camera)
public:
    Camera(std::string &&initialName = "Camera", Thingy* h = nullptr, Window* w = nullptr, float fov = 90.f, int dw = 1024, int dh = 768, float n = .1f, float f = 1024.f);
    virtual ~Camera();
    
    virtual void serialize(Archive& archive) override;
    virtual void init() override;

    static glm::mat4 getProjectionMatrix();
    static glm::mat4 getViewMatrix();
    static void renderAll();
    void render();

private: 
    Thingy *host;
    Window *window; 
    static inline glm::mat4 projectionMatrix;
    static inline glm::mat4 viewMatrix;
	float FoV;
    int width;
    int height;
    float near;
    float far;

protected:
    static inline std::set<Camera *> cameras;
};


#endif

#include "cameraComponent.hpp"
#include "skyRendererComponent.hpp"
#include "objectRendererComponent.hpp"
#include "../physics/physicsComponent.hpp"

CLASS_DEFINITION(Component, Camera)


Camera::Camera(std::string && initialValue, Thingy* h, Window *w, float fov, int dw, int dh, float n, float f) 
    : Component(std::move(initialValue)), host(h), window(w), FoV(fov), width(dw), height(dh), near(n), far(f) {}
    
Camera::~Camera() {
    std::cout << "erasing camera from map" << std::endl;
    cameras.erase(this);
}

void Camera::init() {
    Component::init();
    cameras.insert(this);
}

void Camera::serialize(Archive& archive) {
    Component::serialize(archive);
    archive &
        host &
        window &
        FoV &
        width &
        height &
        near &
        far;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}


void Camera::renderAll() {
    std::cout << "cameras: " << cameras.size() << std::endl;

    for (auto && camera : cameras) {
        if (!camera) {
            std::cout << "skipping camera" << std::endl;
            continue;
        }
           
        camera->render();
    }
    std::cout << "done rendering" << std::endl;
    glfwPollEvents();
}

void Camera::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetWindowSize(&window->getGLFWwindow(), &width, &height);
    glViewport(0, 0, width, height);

    Transform* transform = &host->getComponent<Transform>();
    if (!transform) {
        transform = &host->addComponent<Transform>();
    }

	  projectionMatrix = glm::perspective(glm::radians(FoV), (float)width/(float)height, near, far);
    // view matrix in the inverse of the camera's transformation matrix
    viewMatrix = glm::inverse(transform->getGlobalMatrix());

    ObjectRenderer::drawAll();
    SkyRenderer::drawAll();
    // Physics::debugDrawAll();

    glfwSwapBuffers(&window->getGLFWwindow());
}

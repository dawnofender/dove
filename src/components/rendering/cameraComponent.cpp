#include "cameraComponent.hpp"
#include "skyRendererComponent.hpp"
#include "objectRendererComponent.hpp"
#include "../physics/physicsComponent.hpp"

CLASS_DEFINITION(Component, Camera)


Camera::Camera(std::string && initialValue, Thingy* h, GLFWwindow *w, float fov, int dw, int dh, float n, float f) 
    : Component(std::move(initialValue)), host(h), window(w), FoV(fov), width(dw), height(dh), near(n), far(f) {
    cameras.push_back(this);
    setActiveWindow(window);
}

Camera::~Camera() {
    cameras.erase(std::remove(cameras.begin(), cameras.end(), this), cameras.end());
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

GLFWwindow* Camera::getActiveWindow() {
    return activeWindow;
};

void Camera::setActiveWindow(GLFWwindow* newActiveWindow) {
    activeWindow = newActiveWindow;
}

void Camera::renderAll() {
    for (auto && camera : cameras) {
        camera->render();
    }
}

void Camera::render() {
    setActiveWindow(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetWindowSize(window, &width, &height);
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
    Physics::debugDrawAll();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

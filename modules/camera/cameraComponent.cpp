#include "cameraComponent.hpp"
#include "../rendering/skyRendererComponent.hpp"
#include "../rendering/modelRendererComponent.hpp"
// #include "../physics/physicsComponent.hpp"

CLASS_DEFINITION(Component, Camera)


Camera::Camera(std::string && initialName, Thingy* h, Window *w, float fov, int dw, int dh, float n, float f) 
    : Component(std::move(initialName)), host(h), window(w), FoV(fov), width(dw), height(dh), near(n), far(f) {}
    
Camera::~Camera() {
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

    for (auto && camera : cameras) {
        if (!camera) {
            std::cerr << "ERROR: Camera::renderAll: missing camera, erasing from map" << std::endl;
                cameras.erase(camera);
            continue;
        }
           
        camera->render();
    }
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
    
    // TODO: use rtti & factory to get an instance of all renderer-drived classes and call drawAll on each of them
    ModelRenderer::drawAll(viewMatrix, projectionMatrix);
    SkyRenderer::drawAll(viewMatrix, projectionMatrix);
    // Physics::debugDrawAll();

    glfwSwapBuffers(&window->getGLFWwindow());
}

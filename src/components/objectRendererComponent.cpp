#include "objectRendererComponent.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include <lib/controls.hpp>

CLASS_DEFINITION(MeshRenderer, ObjectRenderer)

ObjectRenderer::ObjectRenderer(std::string &&initialValue, Thingy *h, std::shared_ptr<Material> s, std::shared_ptr<MeshData> m)
    : MeshRenderer(std::move(initialValue), s, m), host(h) {
    
    if (host) transform = &host->getComponent<Transform>();
    renderers.push_back(this);
}

ObjectRenderer::~ObjectRenderer() {
    deleteBuffers();
    renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
}

// ObjectRenderer Component
void ObjectRenderer::drawAll() {
    for (auto &renderer : renderers) {
        renderer->draw();
    }
    unbindBufferData();
}

void ObjectRenderer::draw() {
    if (!transform) {
        std::cout << host->getName() + ": transform not found, attempting fix" << std::endl;
        transform = &host->getComponent<Transform>();
        return;
    }


    // FIX: if this returns false, there was an error, we can skip the object and render all errored objects last with the same shader 
    material->Activate(transform->getMatrix());
    bindBufferData();
    std::cout << "drawing elements" << std::endl;
    std::cout << mesh->indices.size() << std::endl;
    for (auto && layer : mesh->layers) {
        std::cout << layer.getElementSize() << std::endl;
        std::cout << layer.data.size() << std::endl;
        std::cout << layer.getSize() << std::endl;
    }
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    std::cout << "drawing elements: done" << std::endl;
}

void ObjectRenderer::setBounds(glm::vec3 a, glm::vec3 b) { bounds = {a, b}; }


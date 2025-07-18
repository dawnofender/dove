#include "modelRendererComponent.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

CLASS_DEFINITION(MeshRenderer, ModelRenderer)


ModelRenderer::ModelRenderer(std::string &&initialName, Thingy *h, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m)
    : MeshRenderer(std::move(initialName), s, m), host(h) {}

void ModelRenderer::serialize(Archive& archive) {
    MeshRenderer::serialize(archive);
    archive & host & bounds;
}

ModelRenderer::~ModelRenderer() {
    deleteBuffers();
    renderers.erase(this);
}

void ModelRenderer::init() {
    MeshRenderer::init();
    if (host) transform = &host->getComponent<Transform>();
    renderers.insert(this);
}


// ModelRenderer Component
void ModelRenderer::drawAll(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    for (auto &renderer : renderers) {
        renderer->draw(viewMatrix, projectionMatrix);
    }
    unbindBufferData();
}

void ModelRenderer::draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    if (!transform) {
        std::cerr << "ERROR: " << host->getName() << ": transform not found, attempting fix" << std::endl;
        transform = &host->getComponent<Transform>();
        return;
    }


    // FIX: if this returns false, there was an error, we can skip the object and render all errored objects last with the same shader 
    material->Activate(transform->getMatrix(), viewMatrix, projectionMatrix);
    bindBufferData();
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

void ModelRenderer::setBounds(glm::vec3 a, glm::vec3 b) { bounds = {a, b}; }


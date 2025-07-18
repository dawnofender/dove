#include "meshRendererComponent.hpp"
#include <typeinfo>

CLASS_DEFINITION(Component, MeshRenderer)


MeshRenderer::MeshRenderer(std::string &&initialName) 
    : Component(std::move(initialName)) {}

MeshRenderer::MeshRenderer(std::string &&initialName, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m)
    : Component(std::move(initialName)), material(s), mesh(m) {}

void MeshRenderer::serialize(Archive& archive) {
    Component::serialize(archive);
    archive & mesh & material;
}


void MeshRenderer::init() {
    setupBufferData();
}

MeshRenderer::~MeshRenderer() {
    deleteBuffers();
}

void MeshRenderer::setupBufferData() {

    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // VBOs
    buffers.resize(mesh->layers.size());

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glGenBuffers(1, &buffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh->layers[i]->getSize(), mesh->layers[i]->getData(), GL_STATIC_DRAW);
    }    

    // EBO
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * mesh->indices.size(), &mesh->indices[0], GL_STATIC_DRAW);
    //                            NOTE: could be optimized if the layer knows the size of its data type

    // unbind buffers to prevent accidentally modifying them
    unbindBufferData();
}

void MeshRenderer::bindBufferData() {
    glBindVertexArray(VertexArrayID);

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glVertexAttribPointer(i, mesh->layers[i]->getElementSize(), GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(i);
    }    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
}

void MeshRenderer::unbindBufferData() {
	  glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::regenerate() {
    buffers.resize(mesh->layers.size());

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glGenBuffers(1, &buffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, mesh->layers[i]->getSize(), mesh->layers[i]->getData(), GL_STATIC_DRAW);
        //                            NOTE: could be optimized if we know the size of the data type
    }    

    // unbind buffers but not vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::draw() {
    material->Activate(glm::mat4(0)); // NOTE: if this returns false, there was an error - later, we can make it skip the object and render all errored objects last with an error shader 
    bindBufferData();
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}
void MeshRenderer::deleteBuffers() {
    glDeleteVertexArrays(1, &VertexArrayID);
    for (auto& buffer : buffers) {
        glDeleteBuffers(1, &buffer);
    }

}
void MeshRenderer::setMaterial(std::shared_ptr<Material> m) {
    material = m;
}


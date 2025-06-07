#include "meshRendererComponent.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include <lib/controls.hpp>

CLASS_DEFINITION(Component, MeshRenderer)


void MeshRenderer::setupBufferData() {

    // VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // VBOs
    buffers.resize(mesh->layers.size());

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glGenBuffers(1, &buffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof( mesh->layers[i].data ), &mesh->layers[i].data[0], GL_STATIC_DRAW);
        //                            NOTE: could be optimized if we know the size of the data type
    }    

    // EBO
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_BUFFER, sizeof(GLuint) * mesh->indices.size(), &mesh->layers[0].data[0], GL_STATIC_DRAW);
    //                            NOTE: could be optimized if the layer knows the size of its data type

    // unbind buffers to prevent accidentally modifying them
    unbindBufferData();

}

void MeshRenderer::bindBufferData() {
    glBindVertexArray(VertexArrayID);

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glVertexAttribPointer(i, mesh->layers[i].size, GL_FLOAT, GL_FALSE, 0, (void *)0);
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

    for (int i = 0; i < mesh->layers.size(); i++ ) {
        glGenBuffers(1, &buffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof( mesh->layers[i].data ), &mesh->layers[i].data[0], GL_STATIC_DRAW);
        //                            NOTE: could be optimized if we know the size of the data type
    }    

    // unbind buffers but not vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::draw() {
    material->Activate(glm::vec4(0));
  

    bindBufferData();

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

void MeshRenderer::deleteBuffers() {
}

void MeshRenderer::setMaterial(std::shared_ptr<Material> m) {
    material = m;
}

void MeshRenderer::setBounds(glm::vec3 a, glm::vec3 b) { bounds = {a, b}; }


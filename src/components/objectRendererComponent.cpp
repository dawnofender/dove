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

CLASS_DEFINITION(Component, ObjectRenderer)


// ObjectRenderer Component
void ObjectRenderer::drawAll() {
    
    // this is dumb and they should be stored somewhere else and set there instead of calling functions from the tutorial library
    for (auto &renderer : renderers) {
        renderer->draw();
    }

    unbindBufferData();
}

void ObjectRenderer::setupBufferData() {

  // VAO
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  // VBOs
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3),
               &mesh->vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->colors.size() * sizeof(glm::vec3),
               &mesh->colors[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->uvs.size() * sizeof(glm::vec2),
               &mesh->uvs[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3),
               &mesh->normals[0], GL_STATIC_DRAW);
  
  // EBO
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0],
               GL_STATIC_DRAW);
  
  // unbind buffers to prevent accidentally modifying them
  unbindBufferData();

}

void ObjectRenderer::bindBufferData() {
  glBindVertexArray(VertexArrayID);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(3);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
}

void ObjectRenderer::unbindBufferData() {
	glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ObjectRenderer::regenerate() {
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3),
               &mesh->vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->colors.size() * sizeof(glm::vec3),
               &mesh->colors[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->uvs.size() * sizeof(glm::vec2),
               &mesh->uvs[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3),
               &mesh->normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

  // unbind buffers ignoring vao
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ObjectRenderer::draw() {
  // later, this could instead use a material object to deal with uniforms
  // so, thingy->meshrenderer->material->shader

    if (!transform) {
        std::cout << host->getName() + ": transform not found, attempting fix" << std::endl;
        transform = &host->getComponent<Transform>();
        return;
    }

    modelMatrix = transform->getMatrix();

    // FIX: if this returns false, there was an error, we can skip the object and render all errored objects last with the same shader 
    material->Activate(modelMatrix);
  

    bindBufferData();

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

void ObjectRenderer::deleteBuffers() {
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteVertexArrays(1, &VertexArrayID);
}

void ObjectRenderer::setMaterial(std::shared_ptr<Material> m) {
    material = m;
}

void ObjectRenderer::setBounds(glm::vec3 a, glm::vec3 b) { bounds = {a, b}; }


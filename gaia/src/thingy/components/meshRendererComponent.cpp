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
#include <src/dasset/dmesh.hpp>
#include <src/dasset/shader.hpp>

CLASS_DEFINITION(Component, MeshRenderer)


// MeshRenderer Component
void MeshRenderer::drawAll() {
  for (auto &renderer : renderers) {
    renderer->draw();
  }
  unbindBufferData();
}

void MeshRenderer::deleteAll() {
  for (auto &renderer : renderers) {
    renderer->deleteBuffers();
  }
}


void MeshRenderer::setupBufferData() {

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

void MeshRenderer::bindBufferData() {
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

void MeshRenderer::unbindBufferData() {
	glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::regenerate() {
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

void MeshRenderer::draw() {
  // later, this could instead use a material object to deal with uniforms
  // so, thingy->meshrenderer->material->shader
  if (!transform) {
    transform = &host->getComponent<Transform>();
    return;
  }

  shader->Activate();
  bindBufferData();
  texture->bind();
  
  // FIX: some of these matrices can be calculated once to be used by all rendering and the player controller
  glm::mat4 ProjectionMatrix = getProjectionMatrix();
  glm::mat4 ViewMatrix = getViewMatrix();
  glm::mat4 ModelMatrix = transform->getMatrix();
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 10000, 0);
  glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


  glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

void MeshRenderer::deleteBuffers() {
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteVertexArrays(1, &VertexArrayID);
}

void MeshRenderer::setBounds(glm::vec3 a, glm::vec3 b) { bounds = {a, b}; }

// temporary until we have proper material implementation
void MeshRenderer::setTexture(std::shared_ptr<Texture> t) {
  texture = t;
}

#include "spriteRendererComponent.hpp"

CLASS_DEFINITION(MeshRenderer, SpriteRenderer)

void SpriteRenderer::bindBufferData() {
  glBindVertexArray(VertexArrayID);

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
}

void SpriteRenderer::draw() {
  // later, this could instead use a material object to deal with uniforms
  // so, thingy->meshrenderer->material->shader

  shader->Activate();
  bindBufferData();
  
  glm::mat4 ProjectionMatrix = getProjectionMatrix();
  glm::mat4 ViewMatrix = getViewMatrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 10000, 0);
  glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


  glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

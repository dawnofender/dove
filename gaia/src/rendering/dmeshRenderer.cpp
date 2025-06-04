#include "dmeshRenderer.hpp"
#include <iostream>
#include <memory>
#include <lib/controls.hpp>
#include <src/dmesh/dmesh.hpp>
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 


void MeshRenderer::setupBufferData() {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->getVertices().size() * sizeof(glm::vec3), &mesh->getVertices()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->getColors().size() * sizeof(glm::vec3), &mesh->getColors()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->getNormals().size() * sizeof(glm::vec3), &mesh->getNormals()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices().size() * sizeof(unsigned int), &mesh->getIndices()[0], GL_STATIC_DRAW);
    
    // unbind buffer so we dont accidentally modify it
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshRenderer::bindBufferData() {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

}

void MeshRenderer::drawMesh() {
    // bind VAO
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    glDrawElements(GL_TRIANGLES, mesh->getindices().size(), GL_UNSIGNED_INT, nullptr);
}

void MeshRenderer::deleteBuffers() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    //glDeleteBuffers(1, &meshRenderer.uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);

}

// meshData& MeshRenderer::getMesh() {
//     return mesh;
// }

std::vector<unsigned int> MeshRenderer::getIndices() {
    return mesh->getindices;
}

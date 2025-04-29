#include "meshRendererComponent.hpp"
#include <iostream>
#include <memory>
#include <src/dmesh/dmesh.hpp>
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 

CLASS_DEFINITION(Component, MeshRenderer)



// void MeshRendererSystem::unregisterRenderer(MeshRenderer* renderer) {
//     renderers.erase(std::remove(renderers.begin(), renderers.end(), renderer), renderers.end());
// }

void MeshRenderer::drawAll() { std::cout << "drawing meshes" << std::endl;
	  std::lock_guard<std::mutex> lock(m_renderers);
    for (auto& renderer : renderers) {
        renderer->drawMesh();
    }
    std::cout << "drawing meshes: done" << std::endl;
}

// void MeshRenderer::updateAll() { std::cout << "updating meshes" << std::endl;
// 	  std::lock_guard<std::mutex> lock(m_renderers);
//     for (auto& renderer : renderers) {
//         switch(renderer->state) {
//             case 1: 
//                 renderer->setupBufferData();    
//                 renderer->state = 0;
//                 break;
//             case 2:
//                 renderer->updateMesh();
//                 renderer->setupBufferData();    
//                 renderer->state = 0;
//                 break;
//         }
//     }
//     std::cout << "updating meshes: done" << std::endl;
// }
// void MeshRenderer::queueUpdate() {
//     update = true;
// }
// std::vector<std::shared_ptr<MeshRenderer>> MeshRenderer::meshRenderers;

// void MeshRenderer::setup(){
//     // meshRenderers.push_back(std::make_shared<MeshRenderer>(this));
// }

// void MeshRenderer::update(){
//     drawMesh();
// }

void MeshRenderer::setupBufferData(std::shared_ptr<MeshData> newMesh) {
    std::cout << "setting up buffer data" << std::endl;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, newMesh->vertices.size() * sizeof(glm::vec3), &newMesh->vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, newMesh->colors.size() * sizeof(glm::vec3), &newMesh->colors[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, newMesh->normals.size() * sizeof(glm::vec3), &newMesh->normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh->indices.size() * sizeof(unsigned int), &newMesh->indices[0], GL_STATIC_DRAW);
    
    std::cout << "setting up buffer data: done" << std::endl;
}

void MeshRenderer::updateBufferData(std::shared_ptr<MeshData> newMesh) {
    std::cout << "updating buffer data" << std::endl;

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	  glBufferData(GL_ARRAY_BUFFER, newMesh->vertices.size() * sizeof(glm::vec3), &newMesh->vertices[0], GL_STATIC_DRAW);

    std::cout << "updating buffer data: 0" << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, newMesh->colors.size() * sizeof(glm::vec3), &newMesh->colors[0], GL_STATIC_DRAW);

    std::cout << "updating buffer data: 1" << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, newMesh->normals.size() * sizeof(glm::vec3), &newMesh->normals[0], GL_STATIC_DRAW);

    std::cout << "updating buffer data: 2" << std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh->indices.size() * sizeof(unsigned int), &newMesh->indices[0], GL_STATIC_DRAW);
    
    std::cout << "updating buffer data: done" << std::endl;
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
    bindBufferData();    
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}

void MeshRenderer::deleteBuffers() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    //glDeleteBuffers(1, &meshRenderer.uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);

}

void MeshRenderer::setBounds(glm::vec3 a, glm::vec3 b) {
    bounds = {a, b};
}

void MeshRenderer::setMesh(std::shared_ptr<MeshData> newMesh) {
    if (!(newMesh->vertices.size() > 0 && 
          newMesh->colors.size() > 0 &&
          newMesh->normals.size() > 0 &&
          newMesh->indices.size() > 0)) return;

    std::cout << newMesh->vertices.size() << std::endl;
    std::cout << newMesh->colors.size()   << std::endl;
    std::cout << newMesh->normals.size()  << std::endl;
    std::cout << newMesh->indices.size()  << std::endl;

    std::cout << newMesh->vertices[0].x << std::endl;
    std::cout << newMesh->colors[0]  .x << std::endl;
    std::cout << newMesh->normals[0] .x << std::endl;
    std::cout << newMesh->indices[0]    << std::endl;

    std::cout << "setting new mesh: 1" << std::endl;
    updateBufferData(std::move(newMesh));

    // mesh = std::make_shared<MeshData>(*newMesh);
    std::cout << "setting new mesh: done" << std::endl;
    // state = 2;
}

void MeshRenderer::setMesh(MeshData newMesh) {
    std::cout << "setting new mesh" << std::endl;
    mesh = std::make_shared<MeshData>(newMesh);
    std::cout << "setting new mesh: done" << std::endl;
    // state = 2;
}
// void MeshRenderer:: updateMesh() {
// 	  std::lock_guard<std::mutex> lock(m_renderers);
//     mesh = meshUpdate;
// }


// std::shared_ptr<MeshData> MeshRenderer::getMesh() {
//     return(mesh);
// }

// std::vector<unsigned int> MeshRenderer::getIndices() {
//     return mesh->indices;
// }



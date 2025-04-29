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

void MeshRenderer::drawAll() {
    std::cout << "drawing meshes.. " << std::endl;
	std::lock_guard<std::mutex> lock(m_renderers);
    for (auto& renderer : renderers) {
        renderer->drawMesh();
    }
    std::cout << "done drawing meshes" << std::endl;
}

void MeshRenderer::updateAll() {
    std::cout << "updating meshes.. " << std::endl;
	std::lock_guard<std::mutex> lock(m_renderers);
    for (auto& renderer : renderers) {
        switch(renderer->state) {
            case 0: 
                renderer->updateMesh();
                renderers.push_back(renderer);
                renderer->state = 1;
                break;
            case 2:
                renderer->updateMesh();
                renderer->state = 1;
        }
    }
    std::cout << "done updating meshes" << std::endl;
}

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

void MeshRenderer::setupBufferData() {
    std::cout << "testa" << std::endl;
    if (
        mesh.vertices.size() <= 0 ||
        mesh.colors.size() <= 0 ||
        mesh.normals.size() <= 0 ||
        mesh.indices.size() <= 0
    ) return;
    std::cout << "testb" << std::endl;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), &mesh.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh.colors.size() * sizeof(glm::vec3), &mesh.colors[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), &mesh.normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);
    
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
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
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

void MeshRenderer::setMesh(MeshData newMesh) {
    
    state = 2;
}

void MeshRenderer::updateMesh() {
	std::lock_guard<std::mutex> lock(m_mesh);
    mesh = MeshData(meshUpdate);
    setupBufferData();
    meshUpdate.clear();
}

MeshData MeshRenderer::getMesh() {
    return(mesh);
}

// std::vector<unsigned int> MeshRenderer::getIndices() {
//     return mesh.indices;
// }



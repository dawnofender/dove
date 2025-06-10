#include "skyRendererComponent.hpp"

CLASS_DEFINITION(MeshRenderer, SkyRenderer)


SkyRenderer::SkyRenderer(std::string &&initialValue, std::shared_ptr<Material> s)
    : MeshRenderer(std::move(initialValue), s, mesh) {

    renderers.push_back(this);
}


void SkyRenderer::drawAll() {

    // NOTE: could move depth mask toggle into material
    glDepthMask(GL_FALSE);

    for (auto &renderer : renderers) {
        renderer->draw();
    }

    glDepthMask(GL_TRUE);
    unbindBufferData();
}

void SkyRenderer::draw() {
    // later, this could instead use a material object to deal with uniforms
    // so, thingy->meshrenderer->material->shader

    // FIX: if this returns false, there was an error, we can skip the object and render all errored objects last with the same shader 
    material->Activate(glm::mat4(0));
  

    bindBufferData();

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}
// std::shared_ptr<MeshData> SkyRenderer::mesh = doveAssets.cube;
// void SkyRenderer::setMesh(std::shared_ptr<MeshData> newMesh) {
//     mesh = newMesh;
// }
//
    std::vector<glm::vec3> cubeVertices = {
        // +x
        glm::vec3(.5f, .5f, .5f),
        glm::vec3(.5f, -.5f, .5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(.5f, .5f, -.5f),
        // -x
        glm::vec3(-.5f, .5f, -.5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(-.5f, .5f, .5f),
        // +y
        glm::vec3(-.5f, .5f, -.5f),
        glm::vec3(-.5f, .5f, .5f),
        glm::vec3(.5f, .5f, .5f),
        glm::vec3(.5f, .5f, -.5f),
        // -y
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(.5f, -.5f, .5f),
        // +z
        glm::vec3(-.5f, .5f, .5f),
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(.5f, -.5f, .5f),
        glm::vec3(.5f, .5f, .5f),
        // -z
        glm::vec3(.5f, .5f, -.5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(-.5f, .5f, -.5f),
    };
    
    std::vector<glm::vec2> cubeUVs =  {
        // +x
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -x
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // +y
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -y
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // +z
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -z
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
    };
    
    // normals:
    std::vector<glm::vec3> cubeNormals = {
        // +x
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        // -x
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        // +y
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        // -y
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        // +z
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        // -z
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
    };
    
    // colors:
    std::vector<glm::vec3> cubeColors = {
        // +x
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -x
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // +y
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -y
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // +z
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -z
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
    };
    
    // indices:
    std::vector<unsigned int> cubeIndices = {
        0,  2,  1,  0,  3,  2,  //+x
        4,  6,  5,  4,  7,  6,  //-x
        8,  10, 9,  8,  11, 10, //+y
        12, 14, 13, 12, 15, 14, //-y
        16, 18, 17, 16, 19, 18, //+z
        20, 22, 21, 20, 23, 22, //-z
    };
    
    std::shared_ptr<MeshData> mesh;
    std::shared_ptr<MeshData> SkyRenderer::mesh;
    mesh.layers.push_back(std::make_unique<MeshLayer>(Vec3Layer(cubeVertices)));
    mesh.layers.push_back(std::make_unique<MeshLayer>(Vec3Layer(cubeColors))); 
    mesh.layers.push_back(std::make_unique<MeshLayer>(Vec2Layer(cubeUVs)));    
    mesh.layers.push_back(std::make_unique<MeshLayer>(Vec3Layer(cubeNormals)));
    mesh.indices = cubeIndices;
    std::shared_ptr<MeshData> SkyRenderer::mesh = mesh;

#ifndef DMESHRENDERER_HPP
#define DMESHRENDERER_HPP

#include <GL/glew.h>
#include <src/dmesh/dmesh.hpp>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 

class MeshRenderer{
private:
    meshData mesh;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    // GLuint uvbuffer;
    GLuint elementbuffer;


public:
    MeshRenderer(meshData m) : mesh(m) {
        setupBufferData();

    }
    
    void bindBufferData();
    void setupBufferData();
    void drawMesh();
    void deleteBuffers();
    meshData getMesh();
    std::vector<unsigned int> getIndices();
};

#endif

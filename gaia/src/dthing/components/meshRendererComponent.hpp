#ifndef DMESHRENDERERCOMPONENT_HPP
#define DMESHRENDERERCOMPONENT_HPP

#include "component.hpp"
#include <GL/glew.h>
#include <src/dmesh/dmesh.hpp>
#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 

class MeshRenderer : public Component {
private:
    std::shared_ptr<meshData> mesh;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    // GLuint uvbuffer;
    GLuint elementbuffer;
    std::pair<glm::vec3, glm::vec3> bounds;
    static std::vector<std::shared_ptr<MeshRenderer>> meshRenderers;

public:
    void setup() override;
    void bindBufferData();
    void setupBufferData();
    void drawMesh();
    void deleteBuffers();
    void setMesh(std::shared_ptr<meshData> m);
    void setBounds(glm::vec3 a, glm::vec3 b);

    std::shared_ptr<meshData> getMesh();
    // std::vector<unsigned int> getIndices();
};

#endif

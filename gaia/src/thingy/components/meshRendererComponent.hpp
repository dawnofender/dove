#ifndef DMESHRENDERERCOMPONENT_HPP
#define DMESHRENDERERCOMPONENT_HPP

#include "component.hpp"
#include <GL/glew.h>
#include <src/dmesh/dmesh.hpp>
#include <vector>
#include <algorithm>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 


class MeshRenderer : public Component {
CLASS_DECLARATION(MeshRenderer)
private:
    std::shared_ptr<meshData> mesh;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    // GLuint uvbuffer;
    GLuint elementbuffer;
    std::pair<glm::vec3, glm::vec3> bounds;
    static inline std::vector<MeshRenderer*> renderers;

public:

    MeshRenderer(std::string && initialValue, std::shared_ptr<meshData> m)
        : Component(std::move(initialValue)), mesh(m) {
        setupBufferData();
        renderers.push_back(this);
    }

    ~MeshRenderer() {
        deleteBuffers();
        renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
    }

    // void update() override;
    void indexSelf();
    void bindBufferData();
    void setupBufferData();
    void drawMesh();
    void deleteBuffers();
    void setMesh(std::shared_ptr<meshData> m);
    void setBounds(glm::vec3 a, glm::vec3 b);
    static void drawAll();

    std::shared_ptr<meshData> getMesh();
    // std::vector<unsigned int> getIndices();
};



#endif

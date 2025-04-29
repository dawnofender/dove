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
#include <mutex>
#include <iostream>


class MeshRenderer : public Component {
CLASS_DECLARATION(MeshRenderer)
private:
    MeshData mesh;
    std::shared_ptr<MeshData> meshUpdate;
    std::mutex m_mesh;

    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    // GLuint uvbuffer;
    GLuint elementbuffer;

    std::pair<glm::vec3, glm::vec3> bounds;
    

    static inline std::vector<MeshRenderer*> renderers;
    static inline std::mutex m_renderers;

protected: 
    uint8_t state = 0;

public:

    MeshRenderer(std::string && initialValue, MeshData m)
        : Component(std::move(initialValue)), meshUpdate(m) {
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
    void setMesh(std::shared_ptr<MeshData> newMesh);
    void updateMesh();
    void setBounds(glm::vec3 a, glm::vec3 b);
    void lock();
    void queueUpdate();
    static void drawAll();
    static void updateAll();

    MeshData getMesh();
    // std::vector<unsigned int> getIndices();
};



#endif

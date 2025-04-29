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
    std::shared_ptr<MeshData> mesh;
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
    uint8_t state = 1;

public:

    MeshRenderer(std::string&& initialValue, std::shared_ptr<MeshData> m)
        : Component(std::move(initialValue)), mesh(std::move(m)) {
        setupBufferData(mesh);
	      std::lock_guard<std::mutex> lock(m_renderers);
        renderers.push_back(this);
    }

    ~MeshRenderer() {
        deleteBuffers();
        renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
    }

    // void update() override;
    void indexSelf();
    void bindBufferData();
    void setupBufferData(std::shared_ptr<MeshData> newMesh);
    void updateBufferData(std::shared_ptr<MeshData> newMesh);
    void drawMesh();
    void deleteBuffers();
    void setMesh(std::shared_ptr<MeshData> newMesh);
    void setMesh(MeshData newMesh);
    void updateMesh();
    void setBounds(glm::vec3 a, glm::vec3 b);
    void lock();
    std::shared_ptr<MeshData> getMesh();

    static void drawAll();
    static void updateAll();
    // std::vector<unsigned int> getIndices();
};



#endif

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
    std::shared_ptr<MeshData> meshRef;
    std::shared_ptr<MeshData> mesh;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    // GLuint uvbuffer;
    GLuint elementbuffer;
    std::pair<glm::vec3, glm::vec3> bounds;
    static inline std::vector<MeshRenderer*> renderers;
	
    std::mutex m;

public: 
    bool update = false;

public:

    MeshRenderer(std::string && initialValue, std::shared_ptr<MeshData> m)
        : Component(std::move(initialValue)), meshRef(m) {
        updateMesh();
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
    void setMesh(std::shared_ptr<MeshData> m);
    void updateMesh();
    void setBounds(glm::vec3 a, glm::vec3 b);
    void lock();
    void queueUpdate();
    static void drawAll();
    static void updateAll();

    std::shared_ptr<MeshData> getMesh();
    // std::vector<unsigned int> getIndices();
};



#endif

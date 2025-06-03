#ifndef DMESHRENDERERCOMPONENT_HPP
#define DMESHRENDERERCOMPONENT_HPP

#include "component.hpp"
#include "../thingy/thingy.hpp"
#include "transformComponent.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include <src/dasset/material.hpp>
#include <src/dasset/mesh.hpp>
#include <lib/controls.hpp>


class MeshRenderer : public Component {
CLASS_DECLARATION(MeshRenderer)
private:


    // maybe make this a vector so people can layer materials? probably not great for performance but could be interesting...
    std::shared_ptr<Material> material;
    std::shared_ptr<MeshData> mesh;

    Transform *transform;
    Thingy *host;

    GLuint VertexArrayID;

    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
    GLuint uvbuffer;
    GLuint elementbuffer;

    glm::mat4 modelMatrix;
    glm::mat4 MVP;

    std::pair<glm::vec3, glm::vec3> bounds;

    
    static inline std::vector<MeshRenderer *> renderers;

protected:
    uint8_t state = 1;

public:
    MeshRenderer() = default;
    MeshRenderer(std::string &&initialValue, Thingy *h, std::shared_ptr<Material> s, std::shared_ptr<MeshData> m)
        : Component(std::move(initialValue)), host(h), material(s), mesh(m) {
    
        transform = &host->getComponent<Transform>();
        setupBufferData();


        renderers.push_back(this);
    }
  
    ~MeshRenderer() {
        deleteBuffers();
        renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
    }

    void setupBufferData();
    void bindBufferData();
    static void unbindBufferData();
    void regenerate();
    void draw();
    void deleteBuffers();
    void setBounds(glm::vec3 a, glm::vec3 b);
    std::shared_ptr<MeshData> getMesh();

    static void drawAll();
    static void deleteAll();
};

#endif

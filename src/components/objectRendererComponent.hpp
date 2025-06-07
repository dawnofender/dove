#ifndef OBJECTRENDERERCOMPONENT_HPP
#define OBJECTRENDERERCOMPONENT_HPP

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


class ObjectRenderer : public Component {
CLASS_DECLARATION(ObjectRenderer)
private:


    // maybe make this a vector so people can layer materials? probably not great for performance but could be interesting...
    std::shared_ptr<Material> material;
    std::shared_ptr<MeshData> mesh;

    Transform *transform;
    Thingy *host;

    GLuint VertexArrayID;

    glm::mat4 modelMatrix;
    glm::mat4 MVP;

    std::pair<glm::vec3, glm::vec3> bounds;

    
    static inline std::vector<ObjectRenderer *> renderers;

protected:
    uint8_t state = 1;

public:
    ObjectRenderer(std::string &&initialValue, Thingy *h = nullptr, std::shared_ptr<Material> s = nullptr, std::shared_ptr<MeshData> m = nullptr)
        : Component(std::move(initialValue)), host(h), material(s), mesh(m) {
        if (host) 
            transform = &host->getComponent<Transform>();
        
        setupBufferData();


        renderers.push_back(this);
    }

    ObjectRenderer(std::string &&initialValue, std::shared_ptr<Material> s = nullptr)
        : Component(std::move(initialValue)), material(s) {
        setupBufferData();
    }
  
    ~ObjectRenderer() {
        deleteBuffers();
        renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
    }

    // void unserialize(std::istream& in) {
    //     in >> value;
    // }

    void setupBufferData();
    void bindBufferData();
    static void unbindBufferData();
    void regenerate();
    void draw();
    void deleteBuffers();
    void setBounds(glm::vec3 a, glm::vec3 b);
    void setMesh(std::shared_ptr<MeshData>);
    void setMaterial(std::shared_ptr<Material>);
    std::shared_ptr<MeshData> getMesh();

    static void drawAll();
    static void deleteAll();
};

#endif

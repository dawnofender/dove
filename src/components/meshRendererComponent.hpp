#ifndef MESHRENDERERCOMPONENT_HPP
#define MESHRENDERERCOMPONENT_HPP

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
    
    GLuint VertexArrayID;
    std::vector<GLuint> buffers;
    GLuint elementBuffer;
    glm::mat4 MVP;
    
protected:
    uint8_t state = 1;

public:
    MeshRenderer(std::string &&initialValue, std::shared_ptr<Material> s = nullptr, std::shared_ptr<MeshData> m = nullptr)
        : Component(std::move(initialValue)), material(s), mesh(m) {
        
        setupBufferData();


        renderers.push_back(this);
    }

    MeshRenderer(std::string &&initialValue, std::shared_ptr<Material> s = nullptr)
        : Component(std::move(initialValue)), material(s) {
        setupBufferData();
    }
  
    ~MeshRenderer() {
        glDeleteVertexArrays(1, &VertexArrayID);
        for (auto& buffer : buffers) {
            glDeleteBuffers(1, &buffer);
        }

    }

    // void unserialize(std::istream& in) {
    //     in >> value;
    // }

    void setupBufferData();
    void bindBufferData();
    static void unbindBufferData();
    void regenerate();
    void draw();
    void setMesh(std::shared_ptr<MeshData>);
    void setMaterial(std::shared_ptr<Material>);
    std::shared_ptr<MeshData> getMesh();

    static void drawAll();
    static void deleteAll();
};

#endif

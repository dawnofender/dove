#ifndef MESHRENDERERCOMPONENT_HPP
#define MESHRENDERERCOMPONENT_HPP

#include "../component.hpp"
#include "../../thingy/thingy.hpp"
#include "../transformComponent.hpp"
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


class MeshRenderer : public Component {
CLASS_DECLARATION(MeshRenderer)
protected:

    // maybe make this a vector so people can layer materials? probably not great for performance but could be interesting...
    std::shared_ptr<Material> material;
    std::shared_ptr<Mesh> mesh;
    
    GLuint VertexArrayID;
    std::vector<GLuint> buffers;
    GLuint elementBuffer;

public:
    MeshRenderer(std::string && initialValue = "MeshRenderer");
    MeshRenderer(std::string && initialValue, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m);

    virtual void init() override;    

    virtual ~MeshRenderer();

    void setupBufferData();
    void bindBufferData();
    static void unbindBufferData();
    void regenerate();
    void draw();
    void setMesh(std::shared_ptr<Mesh>);
    void setMaterial(std::shared_ptr<Material>);
    std::shared_ptr<Mesh> getMesh();
    void deleteBuffers();

    static void drawAll();
    static void deleteAll();

    virtual void serialize(Archive& archive) override;
};

#endif

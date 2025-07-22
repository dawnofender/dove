#ifndef MESHRENDERERCOMPONENT_HPP
#define MESHRENDERERCOMPONENT_HPP

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// TODO: 
//  - no way all those includes are necessary 
//  - move actual rendering out into another class..? like a master renderer that just gets information from all the meshrenderers
//      - oh hey, a use for "perception" would you look at that
//  - yeah this just takes some weight off of, say, a server with no display
//  - wait, then maybe we don't even have to confuse people with the term "renderer" being in every object. just attach the model and the materials and you're good to go
//      name ideas:
//      - graphics object : descriptive
//      - object data     : meh
//      - model           : simple
//      or just 3D model idk
//  - this does mean that whatever object actually draws everything will have to keep track of data for everything, like how this component saves the id and buffers, adding a good bit of complexity
//  - still think its worth it. especially in the case of an object that doesn't change. why store any kind of functionality for it, like we're doing here? just store the data, and have the client render it

#include "../transform/transformComponent.hpp"
#include "../../core/thingy/thingy.hpp"
#include "../asset/material.hpp"
#include "../asset/mesh.hpp"


class MeshRenderer : public Component {
CLASS_DECLARATION(MeshRenderer)
protected:

    std::shared_ptr<Material> material;
    std::shared_ptr<Mesh> mesh;
    
    GLuint VertexArrayID;
    std::vector<GLuint> buffers;
    GLuint elementBuffer;

public:
    MeshRenderer(std::string && initialName = "MeshRenderer");
    MeshRenderer(std::string && initialName, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m);

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

#ifndef OBJECTRENDERERCOMPONENT_HPP
#define OBJECTRENDERERCOMPONENT_HPP

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
#include "meshRendererComponent.hpp"


class ObjectRenderer : public MeshRenderer {
CLASS_DECLARATION(ObjectRenderer)
private:


    // maybe make this a vector so people can layer materials? probably not great for performance but could be interesting...
    Transform *transform;
    Thingy *host;

    std::pair<glm::vec3, glm::vec3> bounds;
    
    static inline std::vector<ObjectRenderer *> renderers;

protected:
    uint8_t state = 1;

public:
    ObjectRenderer(std::string &&initialValue = "ObjectRenderer", Thingy *h = nullptr, std::shared_ptr<Material> s = nullptr, std::shared_ptr<MeshData> m = nullptr);

    ~ObjectRenderer();
    void draw();
    void setBounds(glm::vec3 a, glm::vec3 b);

    static void drawAll();
    static void deleteAll();
};

#endif

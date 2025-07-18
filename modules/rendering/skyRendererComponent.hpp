#ifndef SKYRENDERERCOMPONENT_HPP
#define SKYRENDERERCOMPONENT_HPP

#include <set>

#include "meshRendererComponent.hpp"



class SkyRenderer : public MeshRenderer {
CLASS_DECLARATION(SkyRenderer)
public: 
    SkyRenderer(std::string &&initialName = "SkyRenderer");
    SkyRenderer(std::string &&initialName, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m);
    
    virtual ~SkyRenderer(); 

    void setupBufferData();
    static void drawAll(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    virtual void init() override;

private: 
    static inline std::set<SkyRenderer *> renderers;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

#endif

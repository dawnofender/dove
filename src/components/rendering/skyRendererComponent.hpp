#ifndef SKYRENDERERCOMPONENT_HPP
#define SKYRENDERERCOMPONENT_HPP

#include "meshRendererComponent.hpp"

#include <set>


class SkyRenderer : public MeshRenderer {
CLASS_DECLARATION(SkyRenderer)
public: 
    SkyRenderer(std::string &&initialName = "SkyRenderer");
    SkyRenderer(std::string &&initialName, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m);
    
    virtual ~SkyRenderer(); 

    static void drawAll();
    void setupBufferData();
    void draw();
    virtual void init() override;

private: 
    static inline std::set<SkyRenderer *> renderers;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

#endif

#ifndef SKYRENDERERCOMPONENT_HPP
#define SKYRENDERERCOMPONENT_HPP

#include "meshRendererComponent.hpp"


class SkyRenderer : public MeshRenderer {
CLASS_DECLARATION(SkyRenderer)
public: 
    SkyRenderer(std::string &&initialValue = "SkyRenderer");
    SkyRenderer(std::string &&initialValue, std::shared_ptr<Material> s, std::shared_ptr<MeshData> m);

    static void drawAll();
    void setupBufferData();
    void draw();
    virtual void load() override;

private: 
    static inline std::vector<SkyRenderer *> renderers;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

#endif

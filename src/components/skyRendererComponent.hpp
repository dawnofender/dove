#ifndef SKYRENDERERCOMPONENT_HPP
#define SKYRENDERERCOMPONENT_HPP

#include "meshRendererComponent.hpp"
// #include <src/dasset/doveAssets.hpp>

class SkyRenderer : public MeshRenderer {
CLASS_DECLARATION(SkyRenderer)
public: 
    SkyRenderer(std::string &&initialValue, std::shared_ptr<Material> s);

    static void drawAll();
    void setupBufferData();
    void draw();


private: 
    static inline std::vector<SkyRenderer *> renderers;
    static std::shared_ptr<MeshData> mesh;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

// std::shared_ptr<MeshData> SkyRenderer::mesh = doveAssets.cube;
// SkyRenderer::setMesh(doveAssets.cube);

#endif

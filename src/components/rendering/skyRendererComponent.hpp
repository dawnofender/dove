#ifndef SKYRENDERERCOMPONENT_HPP
#define SKYRENDERERCOMPONENT_HPP

#include "meshRendererComponent.hpp"


class SkyRenderer : public MeshRenderer {
CLASS_DECLARATION(SkyRenderer)
public: 
    SkyRenderer(std::string &&initialValue = "SkyRenderer", std::shared_ptr<Material> s = nullptr, std::shared_ptr<MeshData> m = nullptr);

    static void drawAll();
    void setupBufferData();
    void draw();


private: 
    static inline std::vector<SkyRenderer *> renderers;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

#endif

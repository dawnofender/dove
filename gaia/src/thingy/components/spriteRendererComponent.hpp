#ifndef SPRITE_RENDERER_COMPONENT_HPP
#define SPRITE_RENDERER_COMPONENT_HPP

#include "meshRendererComponent.hpp"

class SpriteRenderer : public MeshRenderer {
  CLASS_DECLARATION(SpriteRenderer)
public: 
  SpriteRenderer(std::string &&initialValue, std::shared_ptr<Shader> s, std::shared_ptr<MeshData> m)
      : Component(std::move(initialValue)), shader(s), mesh(m) {
    setupBufferData();

    MatrixID = glGetUniformLocation(shader->ID, "MVP");                     
    ViewMatrixID = glGetUniformLocation(shader->ID, "V");                         
    ModelMatrixID = glGetUniformLocation(shader->ID, "M");                       
    LightID = glGetUniformLocation(shader->ID, "LightPosition_worldspace");

    renderers.push_back(this);

  }


}


#endif

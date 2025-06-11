#ifndef DSPRITERENDERERCOMPONENT_HPP
#define DSPRITERENDERERCOMPONENT_HPP

#include "meshRendererComponent.hpp"
#include "component.hpp"

class SpriteRenderer : public MeshRenderer {
  CLASS_DECLARATION(SpriteRenderer)
private: 
  std::shared_ptr<Shader> shader;
  std::shared_ptr<MeshData> mesh;

  GLuint VertexArrayID;

  GLuint vertexbuffer;
  GLuint colorbuffer;
  GLuint normalbuffer;
  GLuint uvbuffer;
  GLuint elementbuffer;

  GLuint MatrixID;       
  GLuint ViewMatrixID;   
  GLuint ModelMatrixID;  

  GLuint LightID;        
  std::pair<glm::vec3, glm::vec3> bounds;

  static inline std::vector<MeshRenderer *> renderers;
  static inline std::mutex m_renderers;
public: 
  SpriteRenderer(std::string &&initialValue = "SpriteRenderer", std::shared_ptr<Shader> s = nullptr, std::shared_ptr<MeshData> m = nullptr)
      : MeshRenderer(std::move(initialValue), s, m) {
    setupBufferData();

    MatrixID = glGetUniformLocation(shader->ID, "MVP");                     
    ViewMatrixID = glGetUniformLocation(shader->ID, "V");                         
    ModelMatrixID = glGetUniformLocation(shader->ID, "M");                       
    LightID = glGetUniformLocation(shader->ID, "LightPosition_worldspace");

    renderers.push_back(this);
  }

  void setupBufferData();
  void bindBufferData();
  void unbindBufferData();
  void regenerateBuffers();
  void draw();
  void deleteBuffers();
  void setBounds(glm::vec3 a, glm::vec3 b);
  std::shared_ptr<MeshData> getMesh();

  static void drawAll();
};


#endif

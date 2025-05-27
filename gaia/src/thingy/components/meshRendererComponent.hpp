#ifndef DMESHRENDERERCOMPONENT_HPP
#define DMESHRENDERERCOMPONENT_HPP

#include "component.hpp"
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

#include <src/dasset/shader.hpp>
#include <src/dasset/dtexture.hpp>
#include <src/dasset/dmesh.hpp>
#include <lib/controls.hpp>


class MeshRenderer : public Component {
  CLASS_DECLARATION(MeshRenderer)
private:
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Texture> texture;
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

protected:
  uint8_t state = 1;

public:
  MeshRenderer(std::string &&initialValue, std::shared_ptr<Shader> s, std::shared_ptr<MeshData> m)
      : Component(std::move(initialValue)), shader(s), mesh(m) {
    setupBufferData();

    MatrixID = glGetUniformLocation(shader->ID, "MVP");                     
    ViewMatrixID = glGetUniformLocation(shader->ID, "V");                         
    ModelMatrixID = glGetUniformLocation(shader->ID, "M");
    LightID = glGetUniformLocation(shader->ID, "LightPosition_worldspace");

    renderers.push_back(this);
  }
  
  ~MeshRenderer() {
    deleteBuffers();
    renderers.erase(std::remove(renderers.begin(), renderers.end(), this), renderers.end());
  }

  void setupBufferData();
  void bindBufferData();
  static void unbindBufferData();
  void regenerate();
  void draw();
  void deleteBuffers();
  void setBounds(glm::vec3 a, glm::vec3 b);
  std::shared_ptr<MeshData> getMesh();

  static void drawAll();
  static void deleteAll();

  // temporary until we have proper material implementation
  void setTexture(std::shared_ptr<Texture> t);
};

#endif

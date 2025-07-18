#ifndef DSHADER_HPP
#define DSHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <lib/shaderLoader.hpp>
#include "asset.hpp"


// TODO: 
//  - A shader is really just the file that contains the shader code, so we could make it inherit from file thing

class Shader : public Asset {
CLASS_DECLARATION(Shader)
public: 

    Shader(std::string && initialName = "Shader");
    Shader(std::string && initialName, std::string && vertexFile, std::string && fragmentFile);
    
    virtual void serialize(Archive& archive) override;
    virtual void init() override;
    
    GLuint ID;
    void Activate();
    void Delete();


private: 
    GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
    std::string vertexFile;
    std::string fragmentFile;
};


#endif

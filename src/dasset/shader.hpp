#ifndef DSHADER_HPP
#define DSHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <lib/shaderLoader.hpp>


class Shader {
private: 
    bool active;
public: 
    GLuint ID;

    Shader(const char* vertexFile, const char* fragmentFile);
    void Activate();
    void Delete();
};


#endif

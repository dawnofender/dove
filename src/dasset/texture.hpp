#ifndef DTEXTURE_HPP
#define DTEXTURE_HPP


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <lib/stb_image.h>


class Texture {
public: 
    GLuint ID;
    GLenum type;
    GLenum unit;

    Texture(const char* image="", GLenum texType=GL_TEXTURE_2D, GLuint slot=0, GLenum format=GL_RGBA, GLenum pixelType=GL_UNSIGNED_BYTE);

    void bind(GLuint unit);
    void unbind(GLuint unit);
    void deleteTexture();
};


#endif

#ifndef DTEXTURE_HPP
#define DTEXTURE_HPP


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <lib/stb_image.h>
#include "asset.hpp"


class Texture : public Asset {
CLASS_DECLARATION(Texture)
public: 
    Texture(std::string && name = "Texture", std::string && i = "", GLenum t = GL_TEXTURE_2D, GLuint s = 0, GLenum f = GL_RGBA, GLenum p = GL_UNSIGNED_BYTE);

    virtual void serialize(Archive& archive) override;
    virtual void init() override;
    
    std::string imageFile;

    GLenum texType;
    GLuint slot;
    GLenum format;
    GLenum pixelType;

    GLuint ID;
    GLenum unit;


    void bind(GLuint unit);
    void unbind(GLuint unit);
    void deleteTexture();
};


#endif

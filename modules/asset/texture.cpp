#include "texture.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CLASS_DEFINITION(Asset, Texture)


Texture::Texture(std::string && name, std::string && i, GLenum t, GLuint s, GLenum f, GLenum p) 
    : Asset(std::move(name)), imageFile(std::move(i)), texType(t), slot(s), format(f), pixelType(p) {
    init(); //TODO: get rid of this and have new assets added in another function that automatically runs init
}

void Texture::serialize(Archive& archive) {
    Asset::serialize(archive);
    archive & 
        imageFile &
        texType &
        slot &
        format &
        pixelType;
}


void Texture::init() {

	  // Stores the width, height, and the number of color channels of the image
	  int widthImg, heightImg, numColCh;
	  // Flips the image so it appears right side up
	  stbi_set_flip_vertically_on_load(true);
	  // Reads the image from a file and stores it in bytes
	  unsigned char* bytes = stbi_load(imageFile.c_str(), &widthImg, &heightImg, &numColCh, 0);

	  // Generates an OpenGL texture object
	  glGenTextures(1, &ID);
	  // Assigns the texture to a Texture Unit
	  glActiveTexture(GL_TEXTURE0 + slot);
	  glBindTexture(texType, ID);

	  // Configures the type of algorithm that is used to make the image smaller or bigger
	  glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	  glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	  // Configures the way the texture repeats (if it does at all)
	  glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	  // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	  // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	  // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	  // Assigns the image to the OpenGL Texture object
	  // FIX: breaks if wrong format is given eg. rgba instead of rgb
	  glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes); 
	  // Generates MipMaps
	  glGenerateMipmap(texType);

	  // Deletes the image data as it is already in the OpenGL Texture object
	  stbi_image_free(bytes);

	  // Unbinds the OpenGL Texture object so that it can't accidentally be modified
	  glBindTexture(texType, 0);
}

void Texture::bind(GLuint unit) {
    // FIX: if texture was never unbinded, this segfaults
    glActiveTexture(GL_TEXTURE0 + unit);
	  glBindTexture(texType, ID);
}

void Texture::unbind(GLuint unit) {
	  glActiveTexture(GL_TEXTURE0 + unit);
	  glBindTexture(texType, 0);
}

void Texture::deleteTexture() {
	  glDeleteTextures(1, &ID);
}

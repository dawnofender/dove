#include "material.hpp"
#include <src/components/rendering/cameraComponent.hpp>



Material::Material(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<Texture>> t) 
    : shader(s), textures(t) {}

bool Material::Activate(glm::mat4 modelMatrix) {

    if (!shader) {
        std::cout << "shader not found" << std::endl;
        return false;
    }
    
    shader->Activate();
    std::shared_ptr<Texture> texture;
    for (int i = 0; i < textures.size(); i++) {
        texture = textures[i];

        if (!texture) {
            std::cout << "texture not found" << std::endl;
            return false;
        }
        texture->bind(i);
    }
    
    // matrix->shader stuff should probably just be handled in the meshrenderer component, 
    // not sure why i moved it here tbh

    MatrixID = glGetUniformLocation(shader->ID, "MVP");                     
    ViewMatrixID = glGetUniformLocation(shader->ID, "view");                         
    ModelMatrixID = glGetUniformLocation(shader->ID, "model");
    ProjectionMatrixID = glGetUniformLocation(shader->ID, "projection");
    LightID = glGetUniformLocation(shader->ID, "LightPosition_worldspace");

    glm::mat4 projectionMatrix = Camera::getProjectionMatrix();
    glm::mat4 viewMatrix = Camera::getViewMatrix();
    glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    glm::vec3 lightPos = glm::vec3(0, 10000, 0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    // TODO: give control of this stuff to the shader 
    // NOTE: depth test is commented out because it would break sky rendering

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL); 
    glEnable(GL_CULL_FACE); 

    return true;
}

std::shared_ptr<Shader> Material::getShader() {
    return shader;
}


void Material::setTexture(int i, std::shared_ptr<Texture> newTexture) {
    if ( i < 0 || i > textures.size()) {
        std::cerr << "Material: tried to set texture at index " << i << ", which is not within the range [0, " << textures.size() << "]. " << std::endl;
        return;
    }
    textures[i] = newTexture;
}

void Material::setCubemap(int i, std::shared_ptr<Texture> newCubemap) {
    if ( i < 0 || i > cubemaps.size()) {
        std::cerr << "Material: tried to set cubemap at index " << i << ", which is not within the range [0, " << cubemaps.size() << "]. " << std::endl;
        return;
    }
    cubemaps[i] = newCubemap;
};


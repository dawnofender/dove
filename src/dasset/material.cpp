#include "material.hpp"



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
    

    MatrixID = glGetUniformLocation(shader->ID, "MVP");                     
    ViewMatrixID = glGetUniformLocation(shader->ID, "V");                         
    ModelMatrixID = glGetUniformLocation(shader->ID, "M");
    LightID = glGetUniformLocation(shader->ID, "LightPosition_worldspace");
    glm::vec3 lightPos = glm::vec3(0, 10000, 0);

    // FIX: store in perception component once that exists
    glm::mat4 projectionMatrix = getProjectionMatrix();
    glm::mat4 viewMatrix = getViewMatrix();
    glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
    
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    return true;
}




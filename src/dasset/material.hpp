#ifndef DMATERIAL_HPP
#define DMATERIAL_HPP

#include "shader.hpp"
#include "texture.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <lib/controls.hpp>

#include <iostream>
    // template<class ComponentType, typename... Args>
    // ComponentType& addComponent(Args&&... args);

    // template<class ComponentType>
    // ComponentType& getComponent();

class Material {
private:
    std::shared_ptr<Shader> shader;
    std::vector<std::shared_ptr<Texture>> textures;
    
    GLuint MatrixID;       
    GLuint ViewMatrixID;   
    GLuint ModelMatrixID;  
    GLuint LightID;        

public:
    Material(std::shared_ptr<Shader> s = nullptr, std::vector<std::shared_ptr<Texture>> t = {nullptr});
    
    bool Activate(glm::mat4 modelMatrix);
    void setTexture(int i, std::shared_ptr<Texture> t);

};


#endif

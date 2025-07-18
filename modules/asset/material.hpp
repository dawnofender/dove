#ifndef DMATERIAL_HPP
#define DMATERIAL_HPP

#include "asset.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <iostream>


class Material : public Asset {
CLASS_DECLARATION(Material)
public:
    Material(std::string && name = "Material", std::shared_ptr<Shader> s = nullptr, std::vector<std::shared_ptr<Texture>> t = {nullptr});
    
    virtual void serialize(Archive& archive) override;

    bool Activate(glm::mat4 modelMatrix = glm::mat4(0), glm::mat4 viewMatrix = glm::mat4(0), glm::mat4 projectionMatrix = glm::mat4(0));
    void setTexture(int i, std::shared_ptr<Texture> newTexture);
    
    // temporary separation of cubemap as its own thing here
    void setCubemap(int i, std::shared_ptr<Texture> newCubemap);
    // TODO: make a class for any kind of shader uniform to alleviate materials of this complexity
    
    void setShader(std::shared_ptr<Shader> newShader);
    
    std::shared_ptr<Shader> getShader();

private:
    std::shared_ptr<Shader> shader;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Texture>> cubemaps;
    
    GLuint MatrixID;       
    GLuint ViewMatrixID;   
    GLuint ProjectionMatrixID;   
    GLuint ModelMatrixID;  
    GLuint LightID;        


};


#endif

#ifndef DEFAULTASSETS_HPP
#define DEFAULTASSETS_HPP

#include <memory>
#include <iostream>

#include <modules/asset/mesh.hpp>
#include <modules/asset/material.hpp>
#include <modules/asset/texture.hpp>
#include <modules/asset/shader.hpp>


// NOTE: extremely temporary dont keep this code for longer than a week

extern std::shared_ptr<Mesh> cube;
 
extern std::shared_ptr<Shader> testShader; 
extern std::shared_ptr<Shader> testShader2; 
extern std::shared_ptr<Shader> testSkyShader;
 
extern std::shared_ptr<Texture> testTexture;
extern std::shared_ptr<Texture> UVGridTexture;
 
extern std::vector<std::shared_ptr<Texture>> testTextureVector; 
extern std::vector<std::shared_ptr<Texture>> testTextureVector2;
 
extern std::shared_ptr<Material> testMaterial;   
extern std::shared_ptr<Material> testMaterial2;  
extern std::shared_ptr<Material> testMaterial3;  
extern std::shared_ptr<Material> testSkyMaterial;

void loadDefaultAssets();


#endif

#ifndef DEFAULTASSETS_HPP
#define DEFAULTASSETS_HPP

#include "dasset/mesh.hpp"
#include "dasset/material.hpp"
#include "dasset/texture.hpp"
#include "dasset/shader.hpp"
#include <memory>
#include <iostream>


// NOTE: extremely temporary dont keep this code for longer than a week

extern std::shared_ptr<MeshData> cube;
 
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

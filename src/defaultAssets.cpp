#include "defaultAssets.hpp"

std::shared_ptr<MeshData> cube;
std::shared_ptr<Shader> testShader; 
std::shared_ptr<Shader> testShader2; 
std::shared_ptr<Shader> testSkyShader;
std::shared_ptr<Texture> testTexture;
std::shared_ptr<Texture> UVGridTexture;
std::vector<std::shared_ptr<Texture>> testTextureVector; 
std::vector<std::shared_ptr<Texture>> testTextureVector2;
std::shared_ptr<Material> testMaterial;   
std::shared_ptr<Material> testMaterial2;  
std::shared_ptr<Material> testMaterial3;  
std::shared_ptr<Material> testSkyMaterial;


void loadDefaultAssets() {
    std::cout << "loading assets..." << std::endl;

    std::vector<glm::vec3> cubeVertices = {
        // +x
        glm::vec3(.5f, .5f, .5f),
        glm::vec3(.5f, -.5f, .5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(.5f, .5f, -.5f),
        // -x
        glm::vec3(-.5f, .5f, -.5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(-.5f, .5f, .5f),
        // +y
        glm::vec3(-.5f, .5f, -.5f),
        glm::vec3(-.5f, .5f, .5f),
        glm::vec3(.5f, .5f, .5f),
        glm::vec3(.5f, .5f, -.5f),
        // -y
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(.5f, -.5f, .5f),
        // +z
        glm::vec3(-.5f, .5f, .5f),
        glm::vec3(-.5f, -.5f, .5f),
        glm::vec3(.5f, -.5f, .5f),
        glm::vec3(.5f, .5f, .5f),
        // -z
        glm::vec3(.5f, .5f, -.5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(-.5f, -.5f, -.5f),
        glm::vec3(-.5f, .5f, -.5f),
    };
    
    std::vector<glm::vec2> cubeUVs = {
        // +x
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -x
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // +y
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -y
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // +z
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        // -z
        glm::vec2(0, 1),
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
    };
    
    // normals:
    std::vector<glm::vec3> cubeNormals = {
        // +x
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),
        // -x
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0),
        // +y
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        // -y
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0),
        // +z
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        // -z
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
    };
    
    // colors:
    std::vector<glm::vec3> cubeColors = {
        // +x
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -x
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // +y
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -y
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // +z
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        // -z
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
    };
    
    // indices:
    std::vector<unsigned int> cubeIndices = {
        0,  1,  2,  0,  2,  3,  //+x
        4,  5,  6,  4,  6,  7,  //-x
        8,  9,  10, 8,  10, 11, //+y
        12, 13, 14, 12, 14, 15, //-y
        16, 17, 18, 16, 18, 19, //+z
        20, 21, 22, 20, 22, 23  //-z
    };
    
    cube = std::make_shared<MeshData>();
    cube->indices = cubeIndices;
    cube->addLayer<Vec3Layer>(cubeVertices);
    cube->addLayer<Vec3Layer>(cubeColors);
    cube->addLayer<Vec2Layer>(cubeUVs);
    cube->addLayer<Vec3Layer>(cubeNormals);
    
    testShader = std::make_shared<Shader>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    testShader2 = std::make_shared<Shader>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    testSkyShader = std::make_shared<Shader>("SkyVertexShader.vertexshader", "SkyboxFragmentShader.fragmentshader");
    
    testTexture = std::make_shared<Texture>("test.png");
    UVGridTexture = std::make_shared<Texture>("../assets/textures/UVgrid.png");
    
    testTextureVector = {testTexture};
    testTextureVector2 = {UVGridTexture};
    
    testMaterial = std::make_shared<Material>(testShader, testTextureVector);
    testMaterial2 = std::make_shared<Material>(testShader2, testTextureVector);
    testMaterial3 = std::make_shared<Material>(testShader, testTextureVector2);
    testSkyMaterial = std::make_shared<Material>(testSkyShader, testTextureVector);
}

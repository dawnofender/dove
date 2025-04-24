#include <cmath>
// #include <glm/detail/qualifier.hpp>
#include <vector>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <thread>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> 

#include <lib/shader.hpp>
#include <lib/texture.hpp>
#include <lib/controls.hpp>
#include <lib/vboindexer.hpp>

#include <lib/SimplexNoise.h>
#include <lib/FastNoise.h>

#include <src/dmesh/dmesh.hpp>
#include <src/thingy/components/meshRendererComponent.hpp>
#include <src/thingy/components/transformComponent.hpp>
#include <src/thingy/components/playerControllerComponent.hpp>
#include <src/thingy/components/gaiaComponent.hpp>
#include <src/thingy/thingy.hpp>


int main(){
    
    // ################
    // # OpenGL setup #
    // ################

    if ( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    
    window = glfwCreateWindow( 1024, 768, "dream", NULL, NULL);
    if (window == NULL){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental=true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
	glClearColor(0.6f, 0.7f, 0.9f, 0.0f);
    glEnable(GL_DEPTH_TEST);    // Enable depth test
    glDepthFunc(GL_LESS);       // Accept fragment if it closer to the camera than the former one
    glEnable(GL_CULL_FACE);     // backface culling


	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
    glUseProgram(programID);
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID); 

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	  
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    //// Initial Field of View
    float initialFoV = 90.0f;
    
    float speed = 32.0f; // 3 units / second
    float mouseSpeed = 0.005f;
    float near = 1.00f;
    float far = 32000000.0f;

    double lastFrameTime = glfwGetTime();
    double lastGenTime = lastFrameTime;
    int nbFrames = 0;


    // ##########
    // # assets #
    // ##########
    
    MeshData cube;

    cube.vertices = {
        // +x    
        glm::vec3(1, 1, 1),
        glm::vec3(1, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 1, 0),            
        // -x    
        glm::vec3(0, 1, 0),                
        glm::vec3(0, 0, 0),                
        glm::vec3(0, 0, 1),                
        glm::vec3(0, 1, 1),                
        // +y    
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(1, 1, 0),            
        // -y    
        glm::vec3(0, 0, 1),                
        glm::vec3(0, 0, 0),                
        glm::vec3(1, 0, 0),                
        glm::vec3(1, 0, 1),                
        // +z    
        glm::vec3(0, 1, 1),                
        glm::vec3(0, 0, 1),                
        glm::vec3(1, 0, 1),                
        glm::vec3(1, 1, 1),                          
        // -z    
        glm::vec3(1, 1, 0),                
        glm::vec3(1, 0, 0),                
        glm::vec3(0, 0, 0),                
        glm::vec3(0, 1, 0),                
    };

    cube.normals = {
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

    cube.colors = {
        // +x    
        glm::vec3(1, 1, 1),
        glm::vec3(1, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 1, 0),            
        // -x    
        glm::vec3(0, 1, 0),                
        glm::vec3(0, 0, 0),                
        glm::vec3(0, 0, 1),                
        glm::vec3(0, 1, 1),                
        // +y    
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(1, 1, 0),            
        // -y    
        glm::vec3(0, 0, 1),                
        glm::vec3(0, 0, 0),                
        glm::vec3(1, 0, 0),                
        glm::vec3(1, 0, 1),                
        // +z    
        glm::vec3(0, 1, 1),                
        glm::vec3(0, 0, 1),                
        glm::vec3(1, 0, 1),                
        glm::vec3(1, 1, 1),                          
        // -z    
        glm::vec3(1, 1, 0),                
        glm::vec3(1, 0, 0),                
        glm::vec3(0, 0, 0),                
        glm::vec3(0, 1, 0),                
    };

    cube.indices = {
        0, 1, 2, 0, 2, 3,       //+x
        4, 5, 6, 4, 6, 7,       //-x
        8, 9, 10, 8, 10, 11,    //+y
        12, 13, 14, 12, 14, 15, //-y
        16, 17, 18, 16, 18, 19, //+z
        20, 21, 22, 20, 22, 23  //-z
    };
    //GLuint Texture = loadDDS("uvtemplate.DDS");
    //GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
   

    // ###############
    // # world setup #
    // ###############
    
    
    Thingy universe("universe");

    Thingy* player = &universe.createChild("player");
    player->addComponent<Transform>("Transform", player);
    player->addComponent<PlayerController>("PlayerController", player);
    
    auto* playerController = &player->getComponent<PlayerController>();


    Thingy* gaia = &universe.createChild("gaia");
    gaia->addComponent<Gaia>("Gaia", gaia, player);

    // put player on serface (temporary)
    playerController->teleport({0, 4210, 0});

    auto* world = &gaia->getComponent<Gaia>();
    Octree cellTree(0, 23);
    world->createWorld(&cellTree);
    // 23 - a bit bigger than earth
    // 65 - max before math breaks at edges (currently breaks world entirely, but can be fixed by generating from the center instead of the corner)
    // 90 - bigger than the observable universe 
    
    // playerController->teleport({0, 7210, 0});
    
    glm::vec3 position;
    universe.addComponent<MeshRenderer>("test", std::make_shared<MeshData>(cube));

    // world->startGeneratingWorld();


    do{
        // measure fps
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastFrameTime >= 1.0 ){ // If last prinf() was more than 1sec ago
           //
   	        // printf and reset
   	        printf("%f ms/frame    ", 1000.0/double(nbFrames));
            std:: cout << playerController->getPosition().x << ", " << playerController->getPosition().y << ", " << playerController->getPosition().z << "\n";
  	        nbFrames = 0;
  	        lastFrameTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);

        // ##########
        // # camera #
        // ##########
        position = playerController->getPosition();
        computeMatricesFromInputs(
            position,
            horizontalAngle,
            verticalAngle,
            initialFoV,
            speed,
            mouseSpeed,
            near,
            far 
        );
        playerController->teleport(position);

        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;        
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(0,10000,0);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        if ( currentTime - lastGenTime >= 30.0 ) {
            lastGenTime += 15.0;

            // for( auto chunkMesh : world.getChunkMeshData() ) {
            //     MeshRenderer chunkMeshRenderer(std::make_shared<meshData>(chunkMesh));
            //     meshRenderers.insert(std::make_shared<MeshRenderer>(chunkMeshRenderer));
            // }

            // for( auto& meshRenderer : meshRenderers ) {
            //     meshRenderer->setupBufferData();
            // }


        }

        MeshRenderer::drawAll();

        // for( auto& meshRenderer : meshRenderers ) {
        //     meshRenderer->drawMesh();
        // }
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);                
        glDisableVertexAttribArray(2);        

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
    // for ( auto& meshRenderer : meshRenderers ) {
    //     meshRenderer->deleteBuffers();
    // }

    glDeleteProgram(programID);
    //glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
	
    return 0;
}

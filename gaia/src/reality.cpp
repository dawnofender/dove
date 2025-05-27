#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
// #include <glm/detail/qualifier.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <lib/controls.hpp>
#include <lib/texture.hpp>
#include <lib/vboindexer.hpp>

#include <lib/FastNoise.h>
#include <lib/SimplexNoise.h>

#include <src/dasset/shader.hpp>
#include <src/dasset/dmesh.hpp>

#include <src/thingy/thingy.hpp>
#include <src/thingy/components/physicsComponent.hpp>
#include <src/thingy/components/sphereColliderComponent.hpp>
#include <src/thingy/components/gaiaComponent.hpp>
#include <src/thingy/components/meshRendererComponent.hpp>
#include <src/thingy/components/playerControllerComponent.hpp>
#include <src/thingy/components/transformComponent.hpp>



GLFWwindow *window;

int main() {
    std::cout << "hi!" << std::endl;

  // ################
  // # OpenGL setup #
  // ################

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  // glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  


  window = glfwCreateWindow(1024, 768, "dream", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have an Intel GPU, they are "
            "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window, 1024 / 2, 768 / 2);
  glClearColor(0.6f, 0.7f, 0.9f, 0.0f);
  glEnable(GL_DEPTH_TEST); // Enable depth test
  glDepthFunc(GL_LESS);   // Accept fragment if it closer to the camera than the
                          // former one
  glEnable(GL_CULL_FACE); // backface culling

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

  std::shared_ptr<MeshData> centerCube = std::make_shared<MeshData>();

  centerCube->vertices = {
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

  centerCube->uvs = {
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
  
  centerCube->normals = {
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

  centerCube->colors = {
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

  centerCube->indices = {
      0,  1,  2,  0,  2,  3,  //+x
      4,  5,  6,  4,  6,  7,  //-x
      8,  9,  10, 8,  10, 11, //+y
      12, 13, 14, 12, 14, 15, //-y
      16, 17, 18, 16, 18, 19, //+z
      20, 21, 22, 20, 22, 23  //-z
  };
  
  std::shared_ptr<MeshData> quad = std::make_shared<MeshData>();

  quad->vertices = {
      glm::vec3(0, 1, 0),
      glm::vec3(0, 0, 0),
      glm::vec3(1, 0, 0),
      glm::vec3(1, 1, 0),
  };
  
  quad->uvs = {
      glm::vec2(0, 1),
      glm::vec2(0, 0),
      glm::vec2(1, 0),
      glm::vec2(1, 1),
  };

  quad->normals = {
      glm::vec3(0, 0, 1),
      glm::vec3(0, 0, 1),
      glm::vec3(0, 0, 1),
      glm::vec3(0, 0, 1),
  };

  quad->colors = {
      glm::vec3(0, 0, 0),
      glm::vec3(1, 0, 0),
      glm::vec3(0, 1, 0),
      glm::vec3(0, 0, 1),
  };

  quad->indices = {
      0, 1, 2, 0, 2, 3
  };

  
  // ###############
  // # world setup #
  // ###############

  Thingy universe("universe");
  universe.addComponent<Physics>("Laws Of Physics");
  

  Thingy *player = &universe.createChild("player");
  Transform playerTransform = player->addComponent<Transform>("Transform");
  PlayerController playerController = player->addComponent<PlayerController>("PlayerController", player);

  

  // ###############
  // # other tests #
  // ###############
    
    auto testShader = std::make_shared<Shader>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    auto testTexture = std::make_shared<Texture>("test.png");
  
  // # GAIA
  
  // Thingy *gaia = &universe.createChild("gaia");
  // gaia->addComponent<Gaia>("Gaia", gaia, player);

  // put player on serface (temporary)
  // playerController->teleport({0, 4210, 0});

  // auto *world = &gaia->getComponent<Gaia>();
  // Octree cellTree(0, 24);
  // world->createWorld(&cellTree);
  // 23 - a bit bigger than earth
  // 65 - max before math breaks at edges (currently breaks world entirely, but
  // can be fixed by generating from the center instead of the corner) 90 -
  // bigger than the observable universe

    // # GLYPHS
  
    Thingy *testCube = &universe.createChild("Cube");
    testCube->addComponent<MeshRenderer>("MeshRenderer", testShader, centerCube);
    testCube->getComponent<MeshRenderer>().setTexture(testTexture);
    
    testCube->addComponent<SphereCollider>("SphereCollider", 0.5f);

    // world->startGeneratingWorld();

    
    // #############
    // # main loop #
    // #############
    
    glm::vec3 position; // temporary for how we're doing the camera right now
    do {
        // #######
        // # fps #
        // #######
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastFrameTime >= 1.0) { // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame    ", 1000.0 / double(nbFrames));
            std::cout << playerTransform.position.x << ", "
                      << playerTransform.position.y << ", "
                      << playerTransform.position.z << "\n";
            nbFrames = 0;
            lastFrameTime += 1.0;
        }

        // #################
        // # prepare frame #
        // #################
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // ##########
        // # camera #
        // ##########
        position = playerTransform.position;
        computeMatricesFromInputs(position, horizontalAngle, verticalAngle,
                                  initialFoV, speed, mouseSpeed, near, far);
        playerTransform.position = position;

        

        // ##############
        // # world loop #
        // ##############

        // random testing stuff: 
        // if ((int)(currentTime * 100)%2 == 0) {
        //   centerCube->vertices[0] += glm::vec3(0, 1, 0);
        // } else {
        //   centerCube->vertices[0] += glm::vec3(0, -1, 0);
        // }
        // universe.getComponent<MeshRenderer>().regenerate();
        //
        // if (currentTime - lastGenTime >= 5.0) {
        //   lastGenTime += 5.0;
        // }

        // updating components:
        MeshRenderer::drawAll();
        Component::updateAll();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    std::cout << "cleaning up..." << std::endl;
    MeshRenderer::deleteAll(); 

    // glDeleteTextures(1, &Texture);

    glfwTerminate();
    std::cout << "bye bye!" << std::endl;

    return 0;
}

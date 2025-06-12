#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
// #include <glm/detail/qualifier.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <functional>
#include <iostream>
#include <sstream>
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
#include <string>

#include <lib/controls.hpp>
#include <lib/texture.hpp>
#include <lib/vboindexer.hpp>

#include <lib/FastNoise.h>
#include <lib/SimplexNoise.h>

#include <src/dasset/shader.hpp>
#include <src/dasset/mesh.hpp>

#include <src/thingy/thingy.hpp>
#include <src/components/cameraComponent.hpp>
#include <src/components/physicsComponent.hpp>
#include <src/components/sphereColliderComponent.hpp>
#include <src/components/boxColliderComponent.hpp>
#include <src/components/rigidBodyComponent.hpp>
// #include <src/components/gaiaComponent.hpp>
#include <src/components/objectRendererComponent.hpp>
#include <src/components/skyRendererComponent.hpp>
#include <src/components/transformComponent.hpp>


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
                   GLFW_OPENGL_COMPAT_PROFILE); // We don't want the old OpenGL
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  


    window = glfwCreateWindow(1024, 768, "dream", NULL, NULL);
    if (window == NULL) {
        // If you have an Intel GPU, they are not 3.3 compatible. 
        fprintf(stderr, "Failed to open GLFW window.\n" );
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
    // # ASSETS #   
    // ##########   
    
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
    
    std::shared_ptr<MeshData> cube = std::make_shared<MeshData>();
    std::cout << "testa" << std::endl;
    cube->indices = cubeIndices;
    cube->addLayer<Vec3Layer>(cubeVertices);
    cube->addLayer<Vec3Layer>(cubeColors);
    cube->addLayer<Vec2Layer>(cubeUVs);
    cube->addLayer<Vec3Layer>(cubeNormals);
    std::cout << "testb" << std::endl;
    
    std::shared_ptr<Shader> testShader = std::make_shared<Shader>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    std::shared_ptr<Shader> testShader2 = std::make_shared<Shader>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    std::shared_ptr<Shader> testSkyShader = std::make_shared<Shader>("SkyVertexShader.vertexshader", "SkyboxFragmentShader.fragmentshader");
    
    std::shared_ptr<Texture> testTexture = std::make_shared<Texture>("test.png");
    std::shared_ptr<Texture> UVGridTexture = std::make_shared<Texture>("../assets/textures/UVgrid.png");
    
    std::vector testTextureVector = {testTexture};
    std::vector testTextureVector2 = {UVGridTexture};
    
    std::shared_ptr<Material> testMaterial = std::make_shared<Material>(testShader, testTextureVector);
    std::shared_ptr<Material> testMaterial2 = std::make_shared<Material>(testShader2, testTextureVector);
    std::shared_ptr<Material> testMaterial3 = std::make_shared<Material>(testShader, testTextureVector2);
    std::shared_ptr<Material> testSkyMaterial = std::make_shared<Material>(testSkyShader, testTextureVector);

    // ###############
    // # world setup #
    // ###############
    
    std::cout << "setting up world" << std::endl;
    // essential thingies
    Thingy universe("universe");
    Physics physics = universe.addComponent<Physics>("Laws Of Physics");

    Thingy *player = &universe.addChild("it's you!");
    // Thingy *sight = &universe.addChild("perception");
    Transform *playerTransform = &player->addComponent<Transform>("Transform", player);
    player->addComponent<SphereCollider>("SphereCollider", 0.5f);
    player->addComponent<RigidBody>("RigidBody", &physics, player, 50.f, true, true);
    
    // main camera
    Thingy *perception = &player->addChild("Perception");
    Transform *perceptionTransform = &perception->addComponent<Transform>("Transform", perception);
    perception->addComponent<Camera>("Camera", perception, window);
    
    // # Basic scene
    // environment:
    Thingy *sky = &universe.addChild("Sky");
    sky->addComponent<Transform>("Transform", sky);
    std::cout << "test0" << std::endl;
    sky->addComponent<SkyRenderer>("SkyRenderer", testSkyMaterial, cube);
    std::cout << "test1" << std::endl;
    
    // ground:
    Thingy *ground = &universe.addChild("Ground");
    Transform *groundTransform = &ground->addComponent<Transform>("Transform", ground);
    groundTransform->setPosition({0, -16.f, 0});
    groundTransform->setScale({32.f, 32.f, 32.f});
    ground->addComponent<BoxCollider>("BoxCollider", ground, glm::vec3(16.f, 16.f, 16.f));
    ground->addComponent<RigidBody>("RigidBody", &physics, ground, 0.f, false, true);
    std::cout << "test0" << std::endl;
    ground->addComponent<ObjectRenderer>("ObjectRenderer", ground, testMaterial3, cube);
  
    std::cout << "test1" << std::endl;
    Thingy *testCube = &universe.addChild("Cube");
    Transform *cubeTransform = &testCube->addComponent<Transform>("Transform", testCube);
    cubeTransform->setPosition({0, 16.f, 0});
    testCube->addComponent<BoxCollider>("BoxCollider", testCube, glm::vec3(.5f, .5f, .5f));
    // testCube->addComponent<SphereCollider>("SphereCollider", 0.5f);
    testCube->addComponent<RigidBody>("RigidBody", &physics, testCube, 1.f);
    testCube->addComponent<ObjectRenderer>("ObjectRenderer", testCube, testMaterial, cube);
    
    Thingy *testCube2 = &universe.addChild("Cube");
    Transform *cubeTransform2 = &testCube2->addComponent<Transform>("Transform", testCube2);
    cubeTransform2->setPosition({0, 16.f, 0});
    testCube2->addComponent<BoxCollider>("BoxCollider", testCube2, glm::vec3(.5f, .5f, .5f));
    testCube2->addComponent<RigidBody>("RigidBody", &physics, testCube2, 1.f);
    testCube2->addComponent<ObjectRenderer>("ObjectRenderer", testCube2, testMaterial2, cube);
    

    std::cout << "setting up world: done" << std::endl;
    // ###############
    // # other tests #
    // ###############
    

    // # GAIA
    
    // Thingy *gaia = &universe.addChild("gaia");
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
    
    // world->startGeneratingWorld();
    
    // #############
    // # main loop #
    // #############
    
    // NOTE: a few parts of this loop should be moved into their own class, such as:
    //  - window info (size etc)
    //  - rendering 
    //  - input (mouse pos, keyboard, click raycasting)

    int screenWidth, screenHeight;
    double mouseX, mouseY;
		
    double deltaTime;
    double lastTime;
    double time;
    
    int frame = 0;
    do {
        // #######
        // # fps #
        // #######
        lastTime = time;
        time = glfwGetTime();
        nbFrames++;
        if ( time - lastFrameTime >= 1.0) { // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame    ", 1000.0 / double(nbFrames));
            std::cout << playerTransform->getPosition().x << ", "
                      << playerTransform->getPosition().y << ", "
                      << playerTransform->getPosition().z << "\n";
            nbFrames = 0;
            lastFrameTime += 1.0;
        }

        // #################
        // # prepare frame #
        // #################
        
        // 
        // ##############
        // # world loop #
        // ##############
        
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
	      glfwGetCursorPos(window, &mouseX, &mouseY);
        // # Player controls
        // these should be moved into components later (input, camera)
        glm::vec3 position = playerTransform->getPosition();
        computeMatricesFromInputs(position, horizontalAngle, verticalAngle,
                                  initialFoV, speed, mouseSpeed, near, far);
        playerTransform->setPosition(position);
        perceptionTransform->setGlobalMatrix(getViewMatrix());

        glViewport(0, 0, screenWidth, screenHeight);
        // raycasting for clicking:
        // The ray Start and End positions, in Normalized Device Coordinates 
        glm::vec4 lRayStart_NDC(
            (mouseX - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
        	  (mouseY - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
        	  -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
        	  1.0f
        );

        glm::vec4 lRayEnd_NDC(
        	  (mouseX - 0.5f) * 2.0f,
        	  (mouseY - 0.5f) * 2.0f,
        	  0.0,
        	  1.0f
        );
        
        glm::mat4 projectionMatrix = getProjectionMatrix();
        glm::mat4 viewMatrix = getViewMatrix();
        glm::mat4 M = glm::inverse(projectionMatrix * viewMatrix);
        glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);
        
        // auto rayCastInfo = physics.rayCast(playerTransform->getPosition(), lRayDir_world, 1000);
        // Thingy* hoverObject = 
        
        // on click, do something to hovered object 
        // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && hoverObject) {
            // std::shared_ptr<
            // if (hoverObject->getComponent<Interact>())
            // Transform *hObjectTransform = &hoverObject->getComponent<Transform>();
            // std::cout << hObjectTransform->getPosition().y << std::endl;
            // hObjectTransform->translate(glm::vec3(0, -1, 0));
            // std::cout << hObjectTransform->getPosition().y << std::endl;
        // }

        // # physics - should be one function in physics component
        deltaTime = time - lastTime;
        RigidBody::syncFromTransforms();
        physics.dynamicsWorld->stepSimulation(deltaTime);
        // physics.dynamicsWorld->updateAabbs();
        // physics.dynamicsWorld->computeOverlappingPairs();
        // physics.dynamicsWorld->performDiscreteCollisionDetection();
        RigidBody::syncToTransforms();

        
        // # updating components
        Component::updateAll();

        // # rendering stuff
        Camera::renderAll();
        frame++;

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    std::cout << "cleaning up..." << std::endl;
    // ObjectRenderer::deleteAll(); 

    // glDeleteTextures(1, &Texture);

    glfwTerminate();
    std::cout << "bye bye!" << std::endl;

    return 0;
}

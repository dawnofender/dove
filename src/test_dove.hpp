#include <lib/setup.hpp>

#include "components/transformComponent.hpp"
#include "components/playerControllerComponent.hpp"
#include "components/physics/physicsComponent.hpp"
#include "components/physics/sphereColliderComponent.hpp"
#include "components/physics/boxColliderComponent.hpp"
#include "components/physics/rigidBodyComponent.hpp"
#include "components/rendering/objectRendererComponent.hpp"
#include "components/rendering/skyRendererComponent.hpp"
#include "components/rendering/cameraComponent.hpp"

#include <string>
#include <numbers> 
#include <format>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>

template <typename T>
void runTest(const std::string testName, T result, T expected) {

    if (result == expected) {
        std::cout << "[PASS] ";
    } else {
        std::cerr << "[FAIL] ";
    }

    std::cout << 
        testName << ": " <<
        std::format("Expected = {}, ", expected) << 
        std::format("Got = {}", result) << 
    std::endl;

}

template <typename T>
void runTestGLM(const std::string testName, T result, T expected) {
    runTest(testName, glm::to_string(result), glm::to_string(expected));
}

int runTests() {

    Thingy *a = new Thingy();
    Thingy *b = &a->addChild();

    Transform *aTransform = &a->addComponent<Transform>("Transform a", a);
    Transform *bTransform = &b->addComponent<Transform>("Transform b", b);
    
    // transformation defaults
    runTestGLM(
        "Transformation: Default position",     // name of test
        bTransform->getPosition(),              // function return value
        glm::vec3(0, 0, 0)                      // expected return value
    );

    // transformation translation
    aTransform->translate(glm::vec3(0, 0, 1));
    runTestGLM(
        "Transformation: Translation", 
        aTransform->getPosition(),
        glm::vec3(0, 0, 1)
    );

    // transformation rotation 
    aTransform->rotate(std::numbers::pi/2.f, glm::vec3(1, 0, 0));
    runTestGLM(
        "Transformation: Rotation", 
        aTransform->getOrientation(),
        glm::quat(0.707107f, 0.707107f, 0.0f, 0.0f) // (sin(pi/2), cos(pi/2), 0, 0)
    );

    // transformation position inheritance
    runTestGLM(
        "Transformation inheritance - Position", 
        bTransform->getGlobalPosition(),
        glm::vec3(0, 0, 1)
    );

    // transformation rotation inheritance
    runTestGLM(
        "Transformation inheritance - Rotation", 
        bTransform->getGlobalOrientation(),
        glm::quat(0.707107f, 0.707107f, 0.0f, 0.0f) 
    );
    
    // transformation scale
    aTransform->setScale(glm::vec3(0.5f, 2.f, 3.f));
    runTestGLM(
        "Transformation: Rotation", 
        aTransform->getScale(),
        glm::vec3(0.5f, 2.f, 3.f)
    );

    // transformation scale inheritance
    runTestGLM(
        "Transformation inheritance - Scale", 
        aTransform->getGlobalScale(),
        glm::vec3(0.5f, 2.f, 3.f)
    );

    return 1;
}

#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "component.hpp"
#include <src/thingy/thingy.hpp>


// TODO: 
//  - set in different spaces: world space, local space, etc. 
//      - I'm like pretty sure this is different from set global because position for example is after orientation
//        whereas with global position takes priority 
//      - actually come to think of it my terminology might be way off here im gonna have to look into that lol
// - make our own matrix and vector classes?

class Transform : public Component {
CLASS_DECLARATION(Transform)
public: 
    glm::mat4 transform;
    Thingy* host;

public: 
    Transform(std::string && initialValue = "Transform", Thingy* h = nullptr, glm::mat4 t = glm::mat4(1))
        : Component(std::move(initialValue)), host(h), transform(t) {
    }


    // setMatrix directly sets the transformation matrix.
    // Output: N/A
    // Inputs: a 4x4 transformation matrix.
    // Example: transform.setMatrix(glm::mat4(1)) sets the transform to the identity matrix.
    void setMatrix(glm::mat4 newTransform);

    void setPosition(glm::vec3 newPos);
    void setScale(glm::vec3 newScale);
    void setOrientation(glm::quat newRotation);
    void setOrigin(glm::mat4 newOrigin);

    void translate(glm::vec3 translation);
    void rotate(float angle, glm::vec3 axis);
    void rotate(glm::quat rotation);
    void rotate(glm::mat4 rotation);
    
    void setGlobalMatrix(glm::mat4 newTransform);
    void setGlobalPosition(glm::vec3 newPos);
    void setGlobalScale(glm::vec3 newScale);
    void setGlobalOrientation(glm::quat newRotation);

    glm::mat4& getMatrix();
    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::quat getOrientation();

    glm::mat4 getGlobalMatrix();
    glm::vec3 getGlobalPosition();
    glm::vec3 getGlobalScale();
    glm::quat getGlobalOrientation();
private:
    glm::mat4 parentTransform;
    void updateChildTransforms();
};


#endif

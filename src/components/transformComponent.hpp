#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "component.hpp"
#include <src/thingy/thingy.hpp>


class Transform : public Component {
CLASS_DECLARATION(Transform)
public: 
    glm::mat4 transform;
    Thingy* host;

public: 
    Transform(std::string && initialValue = "Transform", Thingy* h = nullptr, glm::mat4 t = glm::mat4(1))
        : Component(std::move(initialValue)), host(h), transform(t) {
    }


    
    void setMatrix(glm::mat4 newTransform);
    void setPosition(glm::vec3 newPos);
    void setScale(glm::vec3 newScale);
    void setOrientation(glm::quat newRotation);
    void setOrigin(glm::mat4 newOrigin);

    void translate(glm::vec3 translation);
    // TODO: implement this function
    //  - use glm::rotate( mat4, angle, x, y, z );
    void rotate(float angle, glm::vec3 axis);
    
    // TEST: we don't know if any this global stuff works yet, except for the matrix
    void setGlobalMatrix(glm::mat4 newTransform);
    void setGlobalPosition(glm::vec3 newPos);
    void setGlobalScale(glm::vec3 newScale);
    void setGlobalOrientation(glm::quat newRotation);

    glm::mat4& getMatrix();
    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::quat getOrientation();

    // TEST: we don't know if any this global stuff works yet, except for the matrix
    glm::mat4 getGlobalMatrix();
    glm::vec3 getGlobalPosition();
    glm::vec3 getGlobalScale();
    glm::quat getGlobalOrientation();
private:
    glm::mat4 origin;
    void updateChildTransforms();
};


#endif

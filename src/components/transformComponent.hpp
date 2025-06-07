#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "component.hpp"


class Transform : public Component {
CLASS_DECLARATION(Transform)
public: 
    glm::mat4 transform;

public: 
    Transform(std::string && initialValue, glm::mat4 t = glm::mat4(1))
        : Component(std::move(initialValue)), transform(t) {
    }


    
    void setMatrix(glm::mat4 newTransform);
    void setPosition(glm::vec3 newPos);
    void translate(glm::vec3 translation);
    void setScale(glm::vec3 newScale);
    void setRotation(glm::quat newRotation);
    
    glm::mat4& getMatrix();
    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::quat getRotation();

};


#endif

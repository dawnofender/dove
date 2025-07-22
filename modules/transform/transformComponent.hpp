#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../core/component/component.hpp"
#include "../../core/thingy/thingy.hpp"


// TODO: 
//  - make our own matrix and vector classes?
//  - parent global transform could be a function that moves into the parent recursively, caching the result, that way we don't have to run the apply child transforms thing every time



// NOTE: 
//  - there's 3 approaches (that I can think of) to getting world transform:
//      - recursively climb up parents to root, multiplying each time
//      - store parent's transform so you just need to do one multiplication
//      - store parent's transform and real transform, which is only calculated once every time transform changes
//  - each approach has different advantages; the first is more memory efficient and the last is less calculations
//  - currently using middle approach but maybe we could switch between them?

class Transform : public Component {
CLASS_DECLARATION(Transform)

public: 

    Transform(std::string && initialName = "Transform", Thingy* h = nullptr, glm::mat4 t = glm::mat4(1));


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
    
    virtual void serialize(Archive& archive) override;

private:
    Thingy* host;
    glm::mat4 transform;
    glm::mat4 parentTransform;
    void updateChildTransforms();
};


#endif

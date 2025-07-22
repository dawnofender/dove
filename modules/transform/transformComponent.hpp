#ifndef DTRANSFORMCOMPONENT_HPP
#define DTRANSFORMCOMPONENT_HPP

#include <core/component/component.hpp>
#include <core/thingy/thingy.hpp>
#include <core/math.hpp>


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

    Transform(std::string && initialName = "Transform", Thingy* h = nullptr, Dove::Matrix4 t = Dove::Matrix4(1));


    // setMatrix directly sets the transformation matrix.
    // Output: N/A
    // Inputs: a 4x4 transformation matrix.
    // Example: transform.setMatrix(Dove::Matrix4(1)) sets the transform to the identity matrix.
    void setMatrix(Dove::Matrix4 newTransform);

    void setPosition(Dove::Vector3 newPos);
    void setScale(Dove::Vector3 newScale);
    void setOrientation(Dove::Quaternion newRotation);
    void setOrigin(Dove::Matrix4 newOrigin);

    void translate(Dove::Vector3 translation);
    void rotate(float angle, Dove::Vector3 axis);
    void rotate(Dove::Quaternion rotation);
    void rotate(Dove::Matrix4 rotation);
    
    void setGlobalMatrix(Dove::Matrix4 newTransform);
    void setGlobalPosition(Dove::Vector3 newPos);
    void setGlobalScale(Dove::Vector3 newScale);
    void setGlobalOrientation(Dove::Quaternion newRotation);

    Dove::Matrix4& getMatrix();
    Dove::Vector3 getPosition();
    Dove::Vector3 getScale();
    Dove::Quaternion getOrientation();

    Dove::Matrix4 getGlobalMatrix();
    Dove::Vector3 getGlobalPosition();
    Dove::Vector3 getGlobalScale();
    Dove::Quaternion getGlobalOrientation();
    
    virtual void serialize(Archive& archive) override;

private:
    Thingy* host;
    Dove::Matrix4 transform;
    Dove::Matrix4 parentTransform;
    void updateChildTransforms();
};


#endif

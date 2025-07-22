#include "transformComponent.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


CLASS_DEFINITION(Component, Transform)


Transform::Transform(std::string && initialName, Thingy* h, Dove::Matrix4 t)
    : Component(std::move(initialName)), host(h), transform(t) {}

void Transform::serialize(Archive& archive) {
    Component::serialize(archive);
    
    archive & host & transform & parentTransform;
}


void Transform::updateChildTransforms() {
    Transform* childTransform;
    if (!host) {
        std::cerr << "ERROR: " << name << ": Host thingy not found" << std::endl;
        return;
    }

    for (auto && child : host->children) {
        if (!child) {
            std::cerr << "ERROR: " << name << ": failed to update child transform, child thingy not found" << std::endl;
            continue;
        }
        if ((childTransform = &child->getComponent<Transform>()))
            childTransform->parentTransform = parentTransform * transform;
            childTransform->updateChildTransforms();
    }
}

void Transform::setMatrix(Dove::Matrix4 newTransform) {
    transform = newTransform;
    updateChildTransforms();
}

void Transform::setGlobalMatrix(Dove::Matrix4 newTransform) {
    setMatrix(newTransform / parentTransform);
}

void Transform::setPosition(Dove::Vector3 newPos) {
    transform[3] = Dove::Vector4(newPos.x, newPos.y, newPos.z, transform[3].w);
    updateChildTransforms();
}

void Transform::setGlobalPosition(Dove::Vector3 newPos) {
    Dove::Matrix4 newGlobalMatrix = getGlobalMatrix();
    newGlobalMatrix[3] = Dove::Vector4(newPos.x, newPos.y, newPos.z, newGlobalMatrix[3].w);
    setGlobalMatrix(newGlobalMatrix);
}


void Transform::translate(Dove::Vector3 translation) {
    transform[3] += Dove::Vector4(translation.x, translation.y, translation.z, 0);
    updateChildTransforms();
}

void Transform::rotate(Dove::Quaternion rotation) {
    transform *= glm::mat4_cast(rotation);
    updateChildTransforms();
}

void Transform::rotate(Dove::Matrix4 rotation) {
    transform *= rotation;
    updateChildTransforms();
}

void Transform::rotate(float angle, Dove::Vector3 axis) {
    transform = glm::rotate(transform, angle, axis);
    updateChildTransforms();
}

void Transform::setScale(Dove::Vector3 newScale) {
    transform = glm::scale(transform, newScale);
    updateChildTransforms();
}

void Transform::setGlobalScale(Dove::Vector3 newScale) {
    setScale(newScale - getGlobalScale());
}

void Transform::setOrientation(Dove::Quaternion newOrientation) {
    transform = glm::translate(Dove::Matrix4(1.0), getPosition()) * 
                glm::scale(Dove::Matrix4(1.f), getScale()) * 
                glm::mat4_cast(newOrientation);
    updateChildTransforms();
}

void Transform::setGlobalOrientation(Dove::Quaternion newOrientation) {
    setOrientation(glm::quat_cast(glm::mat4_cast(newOrientation) / parentTransform));
}


Dove::Matrix4& Transform::getMatrix() {
    return transform;
}

Dove::Vector3 Transform::getPosition() {
    return Dove::Vector3(transform[3]);
}

Dove::Vector3 Transform::getScale() {
    Dove::Matrix3 upper3x3(transform);
    return Dove::Vector3(
        glm::length(upper3x3[0]),
        glm::length(upper3x3[1]),
        glm::length(upper3x3[2])
    );
}

Dove::Quaternion Transform::getOrientation() {
    return glm::quat_cast(transform);
}


Dove::Matrix4 Transform::getGlobalMatrix() {
    return parentTransform * transform;
}

Dove::Vector3 Transform::getGlobalPosition() {
    return Dove::Vector3(getGlobalMatrix()[3]);
}

Dove::Quaternion Transform::getGlobalOrientation() {
    return glm::quat_cast(getGlobalMatrix());
}

Dove::Vector3 Transform::getGlobalScale() {
    Dove::Matrix3 upper3x3(getGlobalMatrix());
    return Dove::Vector3(
        glm::length(upper3x3[0]),
        glm::length(upper3x3[1]),
        glm::length(upper3x3[2])
    );
}

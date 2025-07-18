#include "transformComponent.hpp"

CLASS_DEFINITION(Component, Transform)


Transform::Transform(std::string && initialName, Thingy* h, glm::mat4 t)
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

void Transform::setMatrix(glm::mat4 newTransform) {
    transform = newTransform;
    updateChildTransforms();
}

void Transform::setGlobalMatrix(glm::mat4 newTransform) {
    setMatrix(newTransform / parentTransform);
}

void Transform::setPosition(glm::vec3 newPos) {
    transform[3] = glm::vec4(newPos.x, newPos.y, newPos.z, transform[3].w);
    updateChildTransforms();
}

void Transform::setGlobalPosition(glm::vec3 newPos) {
    glm::mat4 newGlobalMatrix = getGlobalMatrix();
    newGlobalMatrix[3] = glm::vec4(newPos.x, newPos.y, newPos.z, newGlobalMatrix[3].w);
    setGlobalMatrix(newGlobalMatrix);
}


void Transform::translate(glm::vec3 translation) {
    transform[3] += glm::vec4(translation.x, translation.y, translation.z, 0);
    updateChildTransforms();
}

void Transform::rotate(glm::quat rotation) {
    transform *= glm::mat4_cast(rotation);
    updateChildTransforms();
}

void Transform::rotate(glm::mat4 rotation) {
    transform *= rotation;
    updateChildTransforms();
}

void Transform::rotate(float angle, glm::vec3 axis) {
    transform = glm::rotate(transform, angle, axis);
    updateChildTransforms();
}

void Transform::setScale(glm::vec3 newScale) {
    transform = glm::scale(transform, newScale);
    updateChildTransforms();
}

void Transform::setGlobalScale(glm::vec3 newScale) {
    setScale(newScale - getGlobalScale());
}

void Transform::setOrientation(glm::quat newOrientation) {
    transform = glm::translate(glm::mat4(1.0), getPosition()) * 
                glm::scale(glm::mat4(1.f), getScale()) * 
                glm::mat4_cast(newOrientation);
    updateChildTransforms();
}

void Transform::setGlobalOrientation(glm::quat newOrientation) {
    setOrientation(glm::quat_cast(glm::mat4_cast(newOrientation) / parentTransform));
}


glm::mat4& Transform::getMatrix() {
    return transform;
}

glm::vec3 Transform::getPosition() {
    return glm::vec3(transform[3]);
}

glm::vec3 Transform::getScale() {
    glm::mat3 upper3x3(transform);
    return glm::vec3(
        glm::length(upper3x3[0]),
        glm::length(upper3x3[1]),
        glm::length(upper3x3[2])
    );
}

glm::quat Transform::getOrientation() {
    return glm::quat_cast(transform);
}


glm::mat4 Transform::getGlobalMatrix() {
    return parentTransform * transform;
}

glm::vec3 Transform::getGlobalPosition() {
    return glm::vec3(getGlobalMatrix()[3]);
}

glm::quat Transform::getGlobalOrientation() {
    return glm::quat_cast(getGlobalMatrix());
}

glm::vec3 Transform::getGlobalScale() {
    glm::mat3 upper3x3(getGlobalMatrix());
    return glm::vec3(
        glm::length(upper3x3[0]),
        glm::length(upper3x3[1]),
        glm::length(upper3x3[2])
    );
}

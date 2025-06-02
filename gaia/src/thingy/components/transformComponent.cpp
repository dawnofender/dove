#include "transformComponent.hpp"

CLASS_DEFINITION(Component, Transform)



void Transform::setMatrix(glm::mat4 newTransform) {
    transform = newTransform;
}


void Transform::setPosition(glm::vec3 newPos) {
    transform[3] = glm::vec4{newPos.x, newPos.y, newPos.z, transform[3].w};
}


void Transform::translate(glm::vec3 translation) {
    transform = glm::translate(transform, translation);
}


void Transform::setScale(glm::vec3 newScale) {
    transform = glm::scale(transform, newScale);
}

void Transform::setRotation(glm::quat newOrientation) {
    transform = glm::translate(glm::mat4(1.0), getPosition()) * 
                glm::scale(glm::mat4(1.f), getScale()) * 
                glm::mat4_cast(newOrientation);
}


glm::mat4 Transform::getMatrix() {
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


glm::quat Transform::getRotation() {
    return glm::quat_cast(transform);
}

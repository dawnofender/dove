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
    glm::vec3 scaleDif = newScale - getScale();
    transform = glm::scale(transform, scaleDif);
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
    glm::vec3 scale = getScale();
    glm::mat3 upper3x3(transform);
    
    return glm::quat(glm::quat_cast(
        // rotation matrix
        glm::mat3(
            upper3x3[0] / scale.x,
            upper3x3[1] / scale.y,
            upper3x3[2] / scale.z
        )
    ));
}

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs(
    glm::vec3 &position, 
    float &horizontalAngle, 
    float &verticalAngle, 
    float initialFoV, 
    float speed, 
    float mouseSpeed, 
    float near, 
    float far
);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif

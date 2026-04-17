#include "Camera.h"

Camera::Camera(){
    this->position = glm::vec4(0, 0, 0, 1);
    this->lookAtTarget = glm::vec4(0, 0, -1, 1);
    this->upVector = glm::vec4(0, 1, 0, 0);
}

void Camera::UpdateFollow(glm::vec3 targetPosition, glm::vec3 targetForward){
    float distance = 1.5f;
    float camera_height = 1.0f;
    float look_at_height = 0.5f;

    glm::vec3 camPos = (targetPosition - targetForward * distance);

    this->position = glm::vec4(camPos.x, camPos.y + camera_height, camPos.z, 1.0f);

    // olhar para o crash, um pouco acima do chão
    this->lookAtTarget = glm::vec4(targetPosition.x, targetPosition.y + look_at_height, targetPosition.z, 1.0f);

    // por enquanto, o up é só o eixo y
    this->upVector = glm::vec4(0, 1, 0, 0);
}

glm::mat4 Camera::GetViewMatrix(){
    glm::vec4 viewVector = this->lookAtTarget - this->position;
    return Matrix_Camera_View(this->position, viewVector, this->upVector);
}
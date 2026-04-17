#pragma once

#include <glm/glm.hpp>
#include "matrices.h"

class Camera {
private:
    glm::vec4 position;
    glm::vec4 lookAtTarget;
    glm::vec4 upVector;

public:
    Camera();
    ~Camera() = default;

    // Atualiza a câmera para perseguir uma entidade
    // Recebe a posição do alvo e o vetor direcional (para onde o alvo está olhando)
    void UpdateFollow(glm::vec3 targetPosition, glm::vec3 targetForward);

    // Retorna a Matriz View para mandarmos para a GPU
    glm::mat4 GetViewMatrix();
};
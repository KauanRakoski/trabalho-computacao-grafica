#include "collisions.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>

// Evitar conflito de macros no Windows
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// Variáveis globais necessárias para DrawDebugAABB
// Estas devem ser declaradas em main.cpp
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;

bool CheckCollisionAABB(AABB a, AABB b) {
    bool overlapX = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);
    bool overlapY = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);
    bool overlapZ = (a.min.z <= b.max.z) && (a.max.z >= b.min.z);

    return overlapX && overlapY && overlapZ;
}

void DrawDebugAABB(AABB bbox)
{
    static GLuint VAO = 0;
    static GLuint VBO = 0;
    static GLuint EBO = 0;

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        GLuint indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            0, 4, 1, 5, 2, 6, 3, 7
        };

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    float vertices[] = {
        bbox.min.x, bbox.min.y, bbox.min.z, 1.0f,
        bbox.max.x, bbox.min.y, bbox.min.z, 1.0f,
        bbox.max.x, bbox.max.y, bbox.min.z, 1.0f,
        bbox.min.x, bbox.max.y, bbox.min.z, 1.0f,
        bbox.min.x, bbox.min.y, bbox.max.z, 1.0f,
        bbox.max.x, bbox.min.y, bbox.max.z, 1.0f,
        bbox.max.x, bbox.max.y, bbox.max.z, 1.0f,
        bbox.min.x, bbox.max.y, bbox.max.z, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindVertexArray(VAO);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniform1i(g_object_id_uniform, 99); 

    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

AABB TransformAABB(glm::vec3 local_min, glm::vec3 local_max, glm::mat4 modelMatrix) {
    glm::vec4 corners[8] = {
        glm::vec4(local_min.x, local_min.y, local_min.z, 1.0f),
        glm::vec4(local_max.x, local_min.y, local_min.z, 1.0f),
        glm::vec4(local_min.x, local_max.y, local_min.z, 1.0f),
        glm::vec4(local_max.x, local_max.y, local_min.z, 1.0f),
        glm::vec4(local_min.x, local_min.y, local_max.z, 1.0f),
        glm::vec4(local_max.x, local_min.y, local_max.z, 1.0f),
        glm::vec4(local_min.x, local_max.y, local_max.z, 1.0f),
        glm::vec4(local_max.x, local_max.y, local_max.z, 1.0f)
    };

    glm::vec3 global_min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 global_max = glm::vec3(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = modelMatrix * corners[i];
        
        global_min.x = std::min(global_min.x, transformed.x);
        global_min.y = std::min(global_min.y, transformed.y);
        global_min.z = std::min(global_min.z, transformed.z);

        global_max.x = std::max(global_max.x, transformed.x);
        global_max.y = std::max(global_max.y, transformed.y);
        global_max.z = std::max(global_max.z, transformed.z);
    }

    return {global_min, global_max};
}

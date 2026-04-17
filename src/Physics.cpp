#include "Physics.h"

bool CheckCollisionAABB(AABB a, AABB b) {
    bool overlapX = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);
    bool overlapY = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);
    bool overlapZ = (a.min.z <= b.max.z) && (a.max.z >= b.min.z);

    return overlapX && overlapY && overlapZ;
}

// AABB TransformAABB(glm::vec3 local_min, glm::vec3 local_max, glm::mat4 modelMatrix) {
//     glm::vec4 corners[8] = {
//         glm::vec4(local_min.x, local_min.y, local_min.z, 1.0f),
//         glm::vec4(local_max.x, local_min.y, local_min.z, 1.0f),
//         glm::vec4(local_min.x, local_max.y, local_min.z, 1.0f),
//         glm::vec4(local_max.x, local_max.y, local_min.z, 1.0f),
//         glm::vec4(local_min.x, local_min.y, local_max.z, 1.0f),
//         glm::vec4(local_max.x, local_min.y, local_max.z, 1.0f),
//         glm::vec4(local_min.x, local_max.y, local_max.z, 1.0f),
//         glm::vec4(local_max.x, local_max.y, local_max.z, 1.0f)
//     };

//     glm::vec3 global_min = glm::vec3(std::numeric_limits<float>::max());
//     glm::vec3 global_max = glm::vec3(std::numeric_limits<float>::lowest());

//     for (int i = 0; i < 8; i++) {
//         glm::vec4 transformed = modelMatrix * corners[i];
        
//         global_min.x = std::min(global_min.x, transformed.x);
//         global_min.y = std::min(global_min.y, transformed.y);
//         global_min.z = std::min(global_min.z, transformed.z);

//         global_max.x = std::max(global_max.x, transformed.x);
//         global_max.y = std::max(global_max.y, transformed.y);
//         global_max.z = std::max(global_max.z, transformed.z);
//     }

//     return {global_min, global_max};
// }
#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <algorithm>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

AABB TransformAABB(glm::vec3 local_min, glm::vec3 local_max, glm::mat4 modelMatrix);

bool CheckCollisionAABB(AABB a, AABB b);
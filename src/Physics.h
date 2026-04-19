#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <algorithm>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};
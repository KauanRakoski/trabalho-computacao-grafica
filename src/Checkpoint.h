#pragma once

#include <glm/glm.hpp>
#include "collisions.h"

class Checkpoint {
public:
    int id;
    glm::vec3 position;
    glm::vec3 size;

    Checkpoint(int id, glm::vec3 pos, glm::vec3 size);

    AABB getAABB() const;
    void draw() const;
};

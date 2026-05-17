#include "Checkpoint.h"

Checkpoint::Checkpoint(int id, glm::vec3 pos, glm::vec3 size)
    : id(id), position(pos), size(size) {
}

AABB Checkpoint::getAABB() const {
    AABB box;
    // Assuming position is the center of the checkpoint
    box.min = position - (size * 0.5f);
    box.max = position + (size * 0.5f);
    return box;
}

void Checkpoint::draw() const {
    DrawDebugAABB(getAABB());
}

#pragma once
#include <glm/vec3.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

bool CheckCollisionAABB(AABB a, AABB b);

class Entity;

void ResolveKartCollision(Entity& k1, Entity& k2, float bump_power);
void ResolveFloorCollision(Entity& entity, AABB floorBox, float floorHeight);

glm::vec3 CalculateBezierTangent(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
glm::vec3 CalculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
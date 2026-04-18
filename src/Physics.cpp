#include "Physics.h"
#include "Entity.h"

bool CheckCollisionAABB(AABB a, AABB b) {
    bool overlapX = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);
    bool overlapY = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);
    bool overlapZ = (a.min.z <= b.max.z) && (a.max.z >= b.min.z);

    return overlapX && overlapY && overlapZ;
}

void ResolveKartCollision(Entity& k1, Entity& k2, float bump_power) {
    AABB box1 = k1.getAABB(0.1f);
    AABB box2 = k2.getAABB(0.1f);
    
    if (CheckCollisionAABB(box1, box2)) {
        glm::vec3 diff = k1.getPosition() - k2.getPosition();
        diff.y = 0.0f;
        
        if (glm::length(diff) > 0.001f) {
            glm::vec3 push_dir = glm::normalize(diff);
            k1.knockback = push_dir * bump_power;
            k2.knockback = -push_dir * bump_power;
        }
    }
}

void ResolveFloorCollision(Entity& entity, AABB floorBox, float floorHeight) {
    if (CheckCollisionAABB(entity.getAABB(0.1f), floorBox)) {
        entity.velocity_y = 0.0f;
        glm::vec3 pos = entity.getPosition();
        entity.setPosition(pos.x, floorHeight, pos.z);
    }
}

// função para calcular um ponto de bezier
glm::vec3 CalculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 p = uuu * p0;
    p += 3.0f * uu * t * p1;
    p += 3.0f * u * tt * p2;
    p += ttt * p3;

    return p;
}

glm::vec3 CalculateBezierTangent(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    float u = 1.0f - t;
    float uu = u * u;
    float tt = t * t;

    glm::vec3 tangent = 3.0f * uu * (p1 - p0);
    tangent += 6.0f * u * t * (p2 - p1);
    tangent += 3.0f * tt * (p3 - p2);

    return glm::normalize(tangent);
}
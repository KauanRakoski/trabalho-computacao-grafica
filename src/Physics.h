#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <algorithm>
#include <vector>

#define NUM_PARTICLE_ELEMENTS 500;
#define PARTICLE_TIME_TO_LIVE = 0.5f;

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float time_to_live;
};

void initParticleSystem();
void spawnParticle(glm::vec3 spawnPosition, std::vector<Particle> &particles, glm::vec3 forwardVector, bool is_going_rear);
void updateParticles(std::vector<Particle> &particles, float deltaTime);
void drawParticles(const std::vector<Particle> &particles, glm::mat4 view, glm::mat4 projection);
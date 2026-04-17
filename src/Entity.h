#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "matrices.h"

class Entity {
protected:
    std::vector<std::string> model_names;
    std::vector<int> shader_ids;

    glm::vec3 position;
    glm::vec3 local_rotation;
    glm::vec3 global_rotation;
    glm::vec3 scale;
    
    float base_facing_y;

public:
    Entity(std::string name, int id);
    Entity(std::vector<std::string> names, std::vector<int> ids);
    virtual ~Entity() = default;

    void setPosition(float x, float y, float z);
    glm::vec3 getPosition();

    void setScale(float x, float y, float z);
    glm::vec3 getScale();

    void setLocalRotation(float x, float y, float z);
    glm::vec3 getLocalRotation();

    void setGlobalRotation(float x, float y, float z);
    glm::vec3 getGlobalRotation();

    void setFacing(float angle_y);
    glm::vec3 getForwardVector();

    glm::mat4 getModelMatrix();
    glm::mat4 getAnchorMatrix();

    virtual void draw(glm::mat4 parent_transform = Matrix_Identity());
};
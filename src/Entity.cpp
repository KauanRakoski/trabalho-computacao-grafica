#include "Entity.h"
#include "Physics.h"
#include <glad/glad.h>

extern void DrawVirtualObject(const char* object_name);
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;

Entity::Entity(std::string name, int id) {
    model_names.push_back(name);
    shader_ids.push_back(id);
    position = glm::vec3(0.0f, 0.0f, 0.0f); 
    local_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    global_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    base_facing_y = 0.0f;
}

Entity::Entity(std::vector<std::string> names, std::vector<int> ids) {
    model_names = names;
    shader_ids = ids;
    position = glm::vec3(0.0f, 0.0f, 0.0f); 
    local_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    global_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    base_facing_y = 0.0f;
}

void Entity::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z); 
}

glm::vec3 Entity::getPosition() {
    return position;
}

void Entity::setScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
}

glm::vec3 Entity::getScale() {
    return scale;
}

void Entity::setLocalRotation(float x, float y, float z) {
    local_rotation = glm::vec3(x, y, z);
}

glm::vec3 Entity::getLocalRotation() {
    return local_rotation;
}

void Entity::setGlobalRotation(float x, float y, float z) {
    global_rotation = glm::vec3(x, y, z);
}

glm::vec3 Entity::getGlobalRotation() {
    return global_rotation;
}

void Entity::setFacing(float angle_y) {
    base_facing_y = angle_y;
}

glm::vec3 Entity::getForwardVector() {
    float total_angle_y = local_rotation.y + base_facing_y;
    float dir_x = -std::sin(total_angle_y);
    float dir_z = -std::cos(total_angle_y);
    return glm::normalize(glm::vec3(dir_x, 0.0f, dir_z));
}

glm::mat4 Entity::getModelMatrix() {
    glm::mat4 model = Matrix_Identity();
    model = model * Matrix_Rotate_Z(global_rotation.z);
    model = model * Matrix_Rotate_Y(global_rotation.y);
    model = model * Matrix_Rotate_X(global_rotation.x);
    model = model * Matrix_Translate(position.x, position.y, position.z);
    model = model * Matrix_Rotate_Y(base_facing_y);
    model = model * Matrix_Rotate_Z(local_rotation.z);
    model = model * Matrix_Rotate_Y(local_rotation.y);
    model = model * Matrix_Rotate_X(local_rotation.x);
    model = model * Matrix_Scale(scale.x, scale.y, scale.z);
    return model;
}

glm::mat4 Entity::getAnchorMatrix() {
    glm::mat4 model = Matrix_Identity();
    model = model * Matrix_Rotate_Z(global_rotation.z);
    model = model * Matrix_Rotate_Y(global_rotation.y);
    model = model * Matrix_Rotate_X(global_rotation.x);
    model = model * Matrix_Translate(position.x, position.y, position.z);
    model = model * Matrix_Rotate_Y(base_facing_y);
    return model;
}

void Entity::draw(glm::mat4 parent_transform) {
    glm::mat4 final_model = parent_transform * getModelMatrix();
    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(final_model));
    
    for (size_t i = 0; i < model_names.size(); i++) {
        glUniform1i(g_object_id_uniform, shader_ids[i]);
        DrawVirtualObject(model_names[i].c_str());
    }
}

void Entity::UpdatePhysics(float deltaTime, float gravity, float friction) {
    velocity_y -= gravity * deltaTime;
    
    knockback -= knockback * friction * deltaTime;
    if (glm::length(knockback) < 0.1f) {
        knockback = glm::vec3(0.0f);
    }
    
    glm::vec3 pos = getPosition();
    pos.y += velocity_y * deltaTime;
    pos.x += knockback.x * deltaTime;
    pos.z += knockback.z * deltaTime;
    
    setPosition(pos.x, pos.y, pos.z);
}

AABB Entity::getAABB(float size) {
    glm::vec3 pos = getPosition();
    AABB box;
    box.min = pos - glm::vec3(size, 0.0f, size);
    box.max = pos + glm::vec3(size, size * 2.0f, size);
    return box;
}
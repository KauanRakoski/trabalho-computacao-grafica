#ifndef TRACKMAP_H
#define TRACKMAP_H

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "collisions.h"
#include <tiny_obj_loader.h>

struct MapShape {
    std::string name;
    size_t first_index;
    size_t num_indices;
    GLuint vao_id;
    AABB bbox;
    GLuint texture_id;
};

struct Triangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

class TrackMap {
public:
    TrackMap(const char* obj_path, const char* base_path, glm::vec3 position, float scale);
    ~TrackMap();

    void Draw();
    
    // Retrieves collisions for walls/objects (AABBs)
    std::vector<AABB> GetCollisions() const;

    // Smooth floor detection: casts a ray down from `world_pos` to find the highest floor triangle
    bool GetFloorHeight(const glm::vec3& world_pos, float& outHeight) const;

private:
    glm::mat4 modelMatrix;
    glm::vec3 position;
    float scale;
    
    std::vector<MapShape> shapes;
    std::vector<Triangle> triangles; // For ray-casting floor detection
    std::map<std::string, GLuint> loadedTextures;

    void LoadTexture(const std::string& filepath, GLuint& texture_id);
    void BuildTriangles(tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& tiny_shapes, std::vector<tinyobj::material_t>& materials, const std::string& base_path);
};

#endif // TRACKMAP_H

#include "TrackMap.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include "matrices.h"
#include "utils.h"

// Note: stbi is already implemented in main or stb_image.cpp
#include <stb_image.h>

extern void DrawVirtualObject(const char* object_name);
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;

// The MAP shader ID we will add to shader_fragment.glsl
#define MAP_SHADER_ID 6

TrackMap::TrackMap(const char* obj_path, const char* base_path, glm::vec3 position, float scale) 
    : position(position), scale(scale) {
    
    modelMatrix = Matrix_Identity();
    modelMatrix = modelMatrix * Matrix_Translate(position.x, position.y, position.z);
    modelMatrix = modelMatrix * Matrix_Scale(scale, scale, scale);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> tiny_shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    printf("Carregando mapa \"%s\"...\n", obj_path);
    bool ret = tinyobj::LoadObj(&attrib, &tiny_shapes, &materials, &warn, &err, obj_path, base_path, true);

    if (!warn.empty()) {
        fprintf(stderr, "WARNING: %s\n", warn.c_str());
    }
    if (!err.empty()) {
        fprintf(stderr, "ERROR: %s\n", err.c_str());
    }

    if (!ret) {
        throw std::runtime_error("Erro ao carregar mapa.");
    }

    BuildTriangles(attrib, tiny_shapes, materials, base_path);
    printf("Mapa carregado. %zu shapes, %zu materiais.\n", shapes.size(), materials.size());
    if (materials.empty()) {
        printf("WARNING: MTL file could not be loaded or parsed.\n");
    }
}

TrackMap::~TrackMap() {
    // Optionally clean up VAOs, VBOs, and Textures
}

void TrackMap::LoadTexture(const std::string& filepath, GLuint& texture_id) {
    if (loadedTextures.find(filepath) != loadedTextures.end()) {
        texture_id = loadedTextures[filepath];
        return;
    }

    // Flip textures for OpenGL
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    if (data == NULL) {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\". Using fallback texture 0.\n", filepath.c_str());
        texture_id = 0; // Fallback
        return;
    }

    printf("Carregando imagem do mapa \"%s\"... OK (%dx%d, %d channels).\n", filepath.c_str(), width, height, channels);

    glGenTextures(1, &texture_id);
    
    // Save previous active texture and bind to 0 temporarily for setup
    GLint last_texture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &last_texture);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    GLenum internalFormat = (channels == 4) ? GL_SRGB8_ALPHA8 : GL_SRGB8;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    loadedTextures[filepath] = texture_id;
    
    // Restore active texture
    glActiveTexture(last_texture);
}

void TrackMap::BuildTriangles(tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& tiny_shapes, std::vector<tinyobj::material_t>& materials, const std::string& base_path) {
    
    struct ShapeGroup {
        GLuint texture_id = 0;
        std::vector<GLuint> indices;
        std::vector<float> model_coefficients;
        std::vector<float> normal_coefficients;
        std::vector<float> texture_coefficients;
        glm::vec3 bbox_min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 bbox_max = glm::vec3(std::numeric_limits<float>::lowest());
    };

    auto ResolveTexturePath = [&](const std::string& texname) {
        std::string full_tex_path = base_path;
        if (!full_tex_path.empty() && full_tex_path.back() != '/' && full_tex_path.back() != '\\') {
            full_tex_path.push_back('/');
        }
        full_tex_path += texname;
        return full_tex_path;
    };

    for (size_t shape = 0; shape < tiny_shapes.size(); ++shape) {
        const auto& tiny_shape = tiny_shapes[shape];
        std::map<int, ShapeGroup> groups;
        size_t num_triangles = tiny_shape.mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            int mat_id = -1;
            if (triangle < tiny_shape.mesh.material_ids.size()) {
                mat_id = tiny_shape.mesh.material_ids[triangle];
            }

            auto& group = groups[mat_id];
            if (group.indices.empty() && group.model_coefficients.empty() && group.texture_coefficients.empty() && group.normal_coefficients.empty()) {
                if (mat_id >= 0 && mat_id < static_cast<int>(materials.size())) {
                    std::string diffuse_tex = materials[mat_id].diffuse_texname;
                    if (!diffuse_tex.empty()) {
                        LoadTexture(ResolveTexturePath(diffuse_tex), group.texture_id);
                    } else {
                        std::string ambient_tex = materials[mat_id].ambient_texname;
                        if (!ambient_tex.empty()) {
                            LoadTexture(ResolveTexturePath(ambient_tex), group.texture_id);
                        }
                    }
                }
            }

            Triangle world_tri;
            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = tiny_shape.mesh.indices[3*triangle + vertex];
                GLuint next_index = static_cast<GLuint>(group.model_coefficients.size() / 4);
                group.indices.push_back(next_index);

                float vx = attrib.vertices[3*idx.vertex_index + 0];
                float vy = attrib.vertices[3*idx.vertex_index + 1];
                float vz = attrib.vertices[3*idx.vertex_index + 2];
                
                glm::vec4 world_v = modelMatrix * glm::vec4(vx, vy, vz, 1.0f);
                if (vertex == 0) world_tri.v0 = glm::vec3(world_v);
                if (vertex == 1) world_tri.v1 = glm::vec3(world_v);
                if (vertex == 2) world_tri.v2 = glm::vec3(world_v);

                group.bbox_min.x = std::min(group.bbox_min.x, world_v.x);
                group.bbox_min.y = std::min(group.bbox_min.y, world_v.y);
                group.bbox_min.z = std::min(group.bbox_min.z, world_v.z);
                group.bbox_max.x = std::max(group.bbox_max.x, world_v.x);
                group.bbox_max.y = std::max(group.bbox_max.y, world_v.y);
                group.bbox_max.z = std::max(group.bbox_max.z, world_v.z);

                group.model_coefficients.push_back(vx);
                group.model_coefficients.push_back(vy);
                group.model_coefficients.push_back(vz);
                group.model_coefficients.push_back(1.0f);

                if (idx.normal_index != -1) {
                    float nx = attrib.normals[3*idx.normal_index + 0];
                    float ny = attrib.normals[3*idx.normal_index + 1];
                    float nz = attrib.normals[3*idx.normal_index + 2];
                    group.normal_coefficients.push_back(nx);
                    group.normal_coefficients.push_back(ny);
                    group.normal_coefficients.push_back(nz);
                    group.normal_coefficients.push_back(0.0f);
                } else {
                    group.normal_coefficients.push_back(0.0f);
                    group.normal_coefficients.push_back(1.0f);
                    group.normal_coefficients.push_back(0.0f);
                    group.normal_coefficients.push_back(0.0f);
                }

                if (idx.texcoord_index != -1) {
                    float u = attrib.texcoords[2*idx.texcoord_index + 0];
                    float v = attrib.texcoords[2*idx.texcoord_index + 1];
                    group.texture_coefficients.push_back(u);
                    group.texture_coefficients.push_back(v);
                } else {
                    group.texture_coefficients.push_back(0.0f);
                    group.texture_coefficients.push_back(0.0f);
                }
            }
            this->triangles.push_back(world_tri);
        }

        for (const auto& group_pair : groups) {
            const auto& group = group_pair.second;
            if (group.indices.empty()) continue;

            GLuint vertex_array_object_id;
            glGenVertexArrays(1, &vertex_array_object_id);
            glBindVertexArray(vertex_array_object_id);

            MapShape map_shape;
            map_shape.name = tiny_shape.name;
            map_shape.first_index = 0;
            map_shape.num_indices = group.indices.size();
            map_shape.vao_id = vertex_array_object_id;
            map_shape.bbox.min = group.bbox_min;
            map_shape.bbox.max = group.bbox_max;
            map_shape.texture_id = group.texture_id;
            shapes.push_back(map_shape);

            GLuint VBO_model, VBO_normal, VBO_texture, EBO;

            glGenBuffers(1, &VBO_model);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_model);
            glBufferData(GL_ARRAY_BUFFER, group.model_coefficients.size() * sizeof(float), group.model_coefficients.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

            if (!group.normal_coefficients.empty()) {
                glGenBuffers(1, &VBO_normal);
                glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
                glBufferData(GL_ARRAY_BUFFER, group.normal_coefficients.size() * sizeof(float), group.normal_coefficients.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(1);
            }

            if (!group.texture_coefficients.empty()) {
                glGenBuffers(1, &VBO_texture);
                glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
                glBufferData(GL_ARRAY_BUFFER, group.texture_coefficients.size() * sizeof(float), group.texture_coefficients.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(2);
            }

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.indices.size() * sizeof(GLuint), group.indices.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    }
}

void TrackMap::Draw() {
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform1i(g_object_id_uniform, MAP_SHADER_ID);

    for (const auto& shape : shapes) {
        glBindVertexArray(shape.vao_id);

        glUniform4f(g_bbox_min_uniform, shape.bbox.min.x, shape.bbox.min.y, shape.bbox.min.z, 1.0f);
        glUniform4f(g_bbox_max_uniform, shape.bbox.max.x, shape.bbox.max.y, shape.bbox.max.z, 1.0f);

        // Bind the specific texture for this shape to GL_TEXTURE0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shape.texture_id);

        glDrawElements(GL_TRIANGLES, shape.num_indices, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}

std::vector<AABB> TrackMap::GetCollisions() const {
    std::vector<AABB> collisions;
    for (const auto& shape : shapes) {
        collisions.push_back(shape.bbox);
    }
    return collisions;
}

// 2D Cross product for barycentric
float sign_2d(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

static glm::vec3 TriangleNormal(const Triangle& tri) {
    glm::vec3 edge1 = tri.v1 - tri.v0;
    glm::vec3 edge2 = tri.v2 - tri.v0;
    glm::vec3 normal = glm::cross(edge1, edge2);
    float len2 = glm::dot(normal, normal);
    if (len2 < 1e-8f) {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }
    return glm::normalize(normal);
}

static float DistancePointSegmentSquaredXZ(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) {
    glm::vec2 ab = b - a;
    float ab_len2 = glm::dot(ab, ab);
    if (ab_len2 == 0.0f) {
        return glm::dot(p - a, p - a);
    }
    float t = glm::dot(p - a, ab) / ab_len2;
    t = std::max(0.0f, std::min(1.0f, t));
    glm::vec2 projection = a + ab * t;
    return glm::dot(p - projection, p - projection);
}

bool PointInTriangleXZ(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec2 p(pt.x, pt.z);
    glm::vec2 a(v1.x, v1.z);
    glm::vec2 b(v2.x, v2.z);
    glm::vec2 c(v3.x, v3.z);
    
    float d1 = sign_2d(p, a, b);
    float d2 = sign_2d(p, b, c);
    float d3 = sign_2d(p, c, a);
    
    bool has_neg = (d1 < 0.0f) || (d2 < 0.0f) || (d3 < 0.0f);
    bool has_pos = (d1 > 0.0f) || (d2 > 0.0f) || (d3 > 0.0f);
    
    return !(has_neg && has_pos);
}

bool TrackMap::ResolveWallCollision(const glm::vec3& oldPos, glm::vec3& newPos, float radius) const {
    const float kHeightTolerance = 0.25f;
    const float kFloorPlaneMargin = 0.15f;
    glm::vec2 newXZ(newPos.x, newPos.z);

    for (const auto& tri : triangles) {
        glm::vec3 normal = TriangleNormal(tri);
        if (std::fabs(normal.y) >= walkable_floor_dot) {
            continue; // Not a steep wall
        }

        float min_x = std::min({tri.v0.x, tri.v1.x, tri.v2.x}) - radius;
        float max_x = std::max({tri.v0.x, tri.v1.x, tri.v2.x}) + radius;
        float min_z = std::min({tri.v0.z, tri.v1.z, tri.v2.z}) - radius;
        float max_z = std::max({tri.v0.z, tri.v1.z, tri.v2.z}) + radius;
        if (newPos.x < min_x || newPos.x > max_x || newPos.z < min_z || newPos.z > max_z) {
            continue;
        }

        float min_y = std::min({tri.v0.y, tri.v1.y, tri.v2.y}) - kHeightTolerance;
        float max_y = std::max({tri.v0.y, tri.v1.y, tri.v2.y}) + kHeightTolerance;
        if (newPos.y < min_y || newPos.y > max_y) {
            continue;
        }

        bool insideXZ = PointInTriangleXZ(newPos, tri.v0, tri.v1, tri.v2);
        float dist2 = insideXZ ? 0.0f : std::min({
            DistancePointSegmentSquaredXZ(newXZ, glm::vec2(tri.v0.x, tri.v0.z), glm::vec2(tri.v1.x, tri.v1.z)),
            DistancePointSegmentSquaredXZ(newXZ, glm::vec2(tri.v1.x, tri.v1.z), glm::vec2(tri.v2.x, tri.v2.z)),
            DistancePointSegmentSquaredXZ(newXZ, glm::vec2(tri.v2.x, tri.v2.z), glm::vec2(tri.v0.x, tri.v0.z))
        });

        if (dist2 > radius * radius) {
            continue;
        }

        if (std::fabs(normal.y) > 0.05f) {
            // If this triangle is a steep ramp under the kart, do not treat it like a wall.
            float planeY = tri.v0.y - (normal.x * (newPos.x - tri.v0.x) + normal.z * (newPos.z - tri.v0.z)) / normal.y;
            if (newPos.y > planeY + kFloorPlaneMargin) {
                continue;
            }
        }

        // Prevent moving into steep geometry
        newPos.x = oldPos.x;
        newPos.z = oldPos.z;
        return true;
    }

    return false;
}

void TrackMap::SetWalkableSlopeDot(float dot) {
    // clamp to [0,1]
    if (dot < 0.0f) dot = 0.0f;
    if (dot > 1.0f) dot = 1.0f;
    // const_cast because member is non-mutable and method is non-const in header
    const_cast<TrackMap*>(this)->walkable_floor_dot = dot;
}

float TrackMap::GetWalkableSlopeDot() const {
    return walkable_floor_dot;
}

bool TrackMap::GetFloorHeight(const glm::vec3& world_pos, float& outHeight) const {
    float highest_y = -99999.0f;
    bool found = false;

    for (const auto& tri : triangles) {
        // Fast AABB check in XZ plane
        float min_x = std::min({tri.v0.x, tri.v1.x, tri.v2.x});
        float max_x = std::max({tri.v0.x, tri.v1.x, tri.v2.x});
        float min_z = std::min({tri.v0.z, tri.v1.z, tri.v2.z});
        float max_z = std::max({tri.v0.z, tri.v1.z, tri.v2.z});

        if (world_pos.x < min_x || world_pos.x > max_x || world_pos.z < min_z || world_pos.z > max_z) {
            continue;
        }

        // Calculate normal to filter out near-vertical triangles from floor detection.
        // Use the same walkable slope threshold as the wall collision logic.
        glm::vec3 normal = TriangleNormal(tri);
        if (std::fabs(normal.y) < walkable_floor_dot) {
            continue; // Too vertical to be a floor
        }

        if (PointInTriangleXZ(world_pos, tri.v0, tri.v1, tri.v2)) {
            // Plane equation to find exact Y
            // N.x * (Px - v0.x) + N.y * (Py - v0.y) + N.z * (Pz - v0.z) = 0
            float Py = tri.v0.y - (normal.x * (world_pos.x - tri.v0.x) + normal.z * (world_pos.z - tri.v0.z)) / normal.y;
            
            // Only consider floors that are near or below the kart's position.
            // A threshold of +0.3f is plenty for going up ramps, without snapping to low roofs.
            if (Py > highest_y && Py < world_pos.y + 0.3f) {
                highest_y = Py;
                found = true;
            }
        }
    }

    if (found) {
        outHeight = highest_y;
        return true;
    }

    return false;
}

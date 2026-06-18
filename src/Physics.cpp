#include "Physics.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <vector>
#include <matrices.h>

static GLuint particleVAO = 0;
static GLuint particleShaderProgram = 0;

static const char* pVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoords;
    out vec2 TexCoords;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        TexCoords = aTexCoords;
        gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    }
)";

static const char* pFragmentShader = R"(
    #version 330 core
    in vec2 TexCoords;
    out vec4 FragColor;
    uniform vec4 color;
    void main() {
        vec2 fromCenter = TexCoords - vec2(0.5, 0.5);
        float dist = length(fromCenter);
        float softEdge = 1.0 - smoothstep(0.0, 0.5, dist);
        FragColor = vec4(color.rgb, color.a * softEdge);
    }
)";

// written by AI
void initParticleSystem() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &pVertexShader, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pFragmentShader, NULL);
    glCompileShader(fragmentShader);

    particleShaderProgram = glCreateProgram();
    glAttachShader(particleShaderProgram, vertexShader);
    glAttachShader(particleShaderProgram, fragmentShader);
    glLinkProgram(particleShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float quadVertices[] = {
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f, 1.0f
    };

    GLuint VBO;
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

static float randomOffset() {
    return ((rand() % 100) / 100.0f) * 0.5f - 0.25f;
}

static int findFirstDeadParticle(const std::vector<Particle> &particles) {
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].time_to_live <= 0.0f) return i;
    }
    return 0;
}

void spawnParticle(glm::vec3 spawnPosition, std::vector<Particle> &particles, glm::vec3 forwardVector, bool is_going_rear) {
    if(particles.empty()) return;
    int p_index = findFirstDeadParticle(particles);
    particles[p_index].time_to_live = 1.0f;
    particles[p_index].position = spawnPosition;
    if (!is_going_rear){
        particles[p_index].velocity = forwardVector + glm::vec3(randomOffset(), 0.2, randomOffset());
    } else {
        particles[p_index].velocity = forwardVector + glm::vec3(randomOffset(), 0.2, randomOffset());

    }
    particles[p_index].color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
}

void updateParticles(std::vector<Particle> &particles, float deltaTime) {
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].time_to_live > 0.0f) {
            particles[i].time_to_live -= deltaTime;
            if (particles[i].time_to_live > 0.0f) {
                particles[i].position += particles[i].velocity * deltaTime;
                particles[i].color.a = particles[i].time_to_live;
            }
        }
    }
}

// written by AI
void drawParticles(const std::vector<Particle> &particles, glm::mat4 view, glm::mat4 projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glUseProgram(particleShaderProgram);
    glBindVertexArray(particleVAO);

    glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].time_to_live > 0.0f) {
            glm::mat4 model = glm::mat4(1.0f);
            model = model * Matrix_Translate(particles[i].position.x, particles[i].position.y, particles[i].position.z);
            
            // Billboarding: make the particle face the camera
            // Expanding smoke effect: scale starts small and grows as it fades
            float particleScale = 0.05f + (1.0f - particles[i].time_to_live) * 0.15f;
            model[0][0] = view[0][0] * particleScale; model[0][1] = view[1][0] * particleScale; model[0][2] = view[2][0] * particleScale;
            model[1][0] = view[0][1] * particleScale; model[1][1] = view[1][1] * particleScale; model[1][2] = view[2][1] * particleScale;
            model[2][0] = view[0][2] * particleScale; model[2][1] = view[1][2] * particleScale; model[2][2] = view[2][2] * particleScale;
            
            glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform4fv(glGetUniformLocation(particleShaderProgram, "color"), 1, glm::value_ptr(particles[i].color));

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
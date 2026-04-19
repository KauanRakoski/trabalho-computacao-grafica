#pragma once

#include <glm/glm.hpp>
#include "Physics.h"

// Função para verificar colisão entre dois AABBs
// Retorna true se os bounding boxes se sobrepõem nos três eixos
bool CheckCollisionAABB(AABB a, AABB b);

// Função para renderizar um AABB em modo de debug
// Desenha um wireframe da caixa delimitadora
void DrawDebugAABB(AABB bbox);

// Função para transformar um AABB local para coordenadas globais
// Usando a matriz de modelo da entidade
AABB TransformAABB(glm::vec3 local_min, glm::vec3 local_max, glm::mat4 modelMatrix);

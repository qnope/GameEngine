#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"
#include <assimp/mesh.h>
#include "../../Tools/glm.h"
#include "../../Tools/geometry.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 tangeant;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
};

AABB &operator<<(AABB &aabb, const Vertex &v);

struct Mesh {
	Mesh(aiMesh const *mesh);

	unsigned materialIndex;
	unsigned numberVertices;
	unsigned numberIndices;
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	AABB aabb;
};

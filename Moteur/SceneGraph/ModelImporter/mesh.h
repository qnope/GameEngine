#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"
#include <assimp/mesh.h>
#include "../../Tools/glm.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 tangeant;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
};

struct AABB {
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{std::numeric_limits<float>::lowest()};
};

struct Cube {
	Cube(const AABB &aabb) {
		center = (aabb.min + aabb.max) / 2.0f;
		diagonal = glm::length(aabb.max - aabb.min);
	}

	Cube(const glm::vec3 &center, float diagonal) : center(center), diagonal(diagonal) {}

	glm::vec3 getLeft() const {
		return center - diagonal / 2.0f;
	}

	glm::vec3 getRight() const {
		return center + diagonal / 2.0f;
	}

	glm::vec3 center;
	float diagonal;
};

AABB &operator<<(AABB &aabb, const Vertex &v);
AABB &operator<<(AABB &aabb, const AABB &a);
AABB operator*(const AABB &aabb, const glm::mat4 &matrix);

struct Mesh {
	Mesh(aiMesh const *mesh);

	unsigned materialIndex;
	unsigned numberVertices;
	unsigned numberIndices;
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	AABB aabb;
};

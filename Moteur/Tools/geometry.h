#pragma once
#include "glm.h"

struct AABB {
    glm::vec3 min{ std::numeric_limits<float>::max() };
    glm::vec3 max{ std::numeric_limits<float>::lowest() };
};

AABB &operator<<(AABB &aabb, const AABB &a);
AABB operator*(const AABB &aabb, const glm::mat4 &matrix);
AABB operator*(const AABB &aabb, const glm::quat &quat);

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
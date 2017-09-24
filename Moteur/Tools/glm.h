#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

inline std::ostream &operator<<(std::ostream &o, const glm::vec2 &v) {
	o << v.x << "," << v.y << std::endl;
	return o;
}

inline std::ostream &operator<<(std::ostream &o, const glm::vec3 &v) {
	o << v.x << "," << v.y << "," << v.z << std::endl;
	return o;
}

inline std::ostream &operator<<(std::ostream &o, const glm::vec4 &v) {
	o << v.x << "," << v.y << "," << v.z << "," << v.w << std::endl;
	return o;
}

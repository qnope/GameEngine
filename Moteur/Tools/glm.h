#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

template<typename T, glm::precision P>
inline std::ostream &operator<<(std::ostream &o, const glm::tvec2<T, P> &v) {
    o << v.x << "," << v.y << std::endl;
    return o;
}

template<typename T, glm::precision P>
inline std::ostream &operator<<(std::ostream &o, const glm::tvec3<T, P> &v) {
    o << v.x << "," << v.y << "," << v.z << std::endl;
    return o;
}

template<typename T, glm::precision P>
inline std::ostream &operator<<(std::ostream &o, const glm::tvec4<T, P> &v) {
    o << v.x << "," << v.y << "," << v.z << "," << v.w << std::endl;
    return o;
}

#include "geometry.h"



AABB & operator<<(AABB & aabb, const AABB & a)
{
    aabb.min = glm::min(aabb.min, a.min);
    aabb.max = glm::max(aabb.max, a.max);

    return aabb;
}

AABB operator*(const AABB & aabb, const glm::mat4 & matrix)
{
    AABB r;
    r.min = glm::vec3(matrix * glm::vec4(aabb.min, 1.0f));
    r.max = glm::vec3(matrix * glm::vec4(aabb.max, 1.0f));
    return r;
}

AABB operator*(const AABB &aabb, const glm::quat &quat) {
    AABB r;
    r.min = glm::vec3(quat * glm::vec4(aabb.min, 1.0f));
    r.max = glm::vec3(quat * glm::vec4(aabb.max, 1.0f));
    return r;
}

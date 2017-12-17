#pragma once

#include "../Tools/glm.h"

struct Camera {
    glm::mat4 getPerspectiveMatrix() const {
        auto perspective{ glm::perspective(fovy, ratio, near, far) };
        perspective[1][1] *= -1;
        return perspective;
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + direction, up);
    }

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };

    float near{ 1.0f };
    float far{ 10000.0f };
    float ratio{ 4.0f / 3.0f };
    float fovy{ glm::radians(70.0f) };
};

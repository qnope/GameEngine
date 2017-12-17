#pragma once

#include <deque>
#include "entity.h"

class EntityManager {
public:
    EntityManager(std::deque<bool> *validityDeque, std::deque<glm::mat4> *matricesDeque, std::deque<AABB> *aabbDeque, const AABB &originalAABB);

    Entity createEntity(glm::mat4 matrice = glm::mat4(1.f));

private:
    std::deque<bool> *mValidityDeque;
    std::deque<glm::mat4> *mMatricesDeque;
    std::deque<AABB> *mAABBsDeque;
    const AABB mOriginalAABB;
};
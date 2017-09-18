#include "entitymanager.h"

EntityManager::EntityManager(std::deque<bool>* validityDeque, std::deque<glm::mat4>* matricesDeque, std::deque<AABB> *aabbsDeque, const AABB &originalAABB) :
	mValidityDeque(validityDeque),
	mMatricesDeque(matricesDeque),
	mAABBsDeque(aabbsDeque),
	mOriginalAABB(originalAABB)
{
}

Entity EntityManager::createEntity(glm::mat4 matrice)
{
	// If one is invalid, we do not need to push one to back
	auto itInvalid = std::find(mValidityDeque->begin(), mValidityDeque->end(), false);

	if (itInvalid != mValidityDeque->end()) {
		auto index = std::distance(mValidityDeque->begin(), itInvalid);
		(*mValidityDeque)[index] = true;
		(*mMatricesDeque)[index] = matrice;
		(*mAABBsDeque)[index] = mOriginalAABB * matrice;
		return Entity{ &(*mValidityDeque)[index], &(*mMatricesDeque)[index], &(*mAABBsDeque)[index], mOriginalAABB};
	}

	mValidityDeque->push_back(true);
	mMatricesDeque->push_back(matrice);
	mAABBsDeque->push_back(mOriginalAABB * matrice);

	return Entity{ &mValidityDeque->back(), &mMatricesDeque->back(), &mAABBsDeque->back(), mOriginalAABB };
}

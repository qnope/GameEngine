#include "dynaobject.h"


DynaObject::DynaObject(Entity* e) {
	mRef = glm::mat4{ 1.f };
	mVisualEntity = e;
}

DynaObject::~DynaObject() {
	delete mVisualEntity;
}

void DynaObject::computeRenderState(float alpha) {
	*mVisualEntity->mMatrix = glm::mix(mPrevState.mRef.mat, mRef.mat, alpha);
	mVisualEntity->mAABB->min = glm::mix(mPrevState.mAABB.min, mAABB.min, alpha);
	mVisualEntity->mAABB->max = glm::mix(mPrevState.mAABB.max, mAABB.max, alpha);
}

void DynaObject::saveState() {
	mPrevState.mAABB = mAABB;
	mPrevState.mRef = mRef;
}

void DynaObject::rotate(const glm::vec3& axis, const float& angle) {
	glm::mat4 rot = glm::mat4{ 1.f };
	rot = glm::rotate(rot, angle, axis);
	mRef = glm::rotate(mRef.mat, angle, axis);
	mAABB = mAABB * rot;
}

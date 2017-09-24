#include "dynaobject.h"


DynaObject::DynaObject(Entity* e) {
	mState.mRef = glm::mat4{ 1.f };
	mVisualEntity = e;
}

DynaObject::~DynaObject() {
	delete mVisualEntity;
}

void DynaObject::computeRenderState(const float alpha) {
	mVisualEntity->computeState(mPrevState, mState, alpha);
}

void DynaObject::saveState() {
	mPrevState.mAABB = mState.mAABB;
	mPrevState.mRef = mState.mRef;
}

void DynaObject::rotate(const glm::vec3& axis, const float& angle) {
	glm::mat4 rot = glm::mat4{ 1.f };
	rot = glm::rotate(rot, angle, axis);
	mState.mRef = glm::rotate(mState.mRef.mat, angle, axis);
	mState.mAABB = mState.mAABB * rot;
}

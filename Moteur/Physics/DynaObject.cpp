#include "dynaobject.h"


DynaObject::DynaObject(Entity* e) {
	mState.mPos = glm::vec3(0.f, 0.f, 0.f);
	mState.mRot = glm::quat(1, 0, 0.f, 0.f);
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
	mPrevState.mRot = mState.mRot;
	mPrevState.mPos = mState.mPos;
}

void DynaObject::rotate(const glm::vec3& axis, const float& angle) {
	glm::mat4 rot = glm::mat4{ 1.f };
	rot = glm::rotate(rot, angle, axis);
	mState.mRot = glm::rotate(mState.mRot, angle, axis);
	mState.mAABB = mState.mAABB * rot;
}

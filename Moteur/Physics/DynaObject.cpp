#include "dynaobject.h"


DynaObject::DynaObject(Entity* e) {
	mState.mPos = glm::vec3(0.f, 0.f, 0.f);
	mState.mRot = glm::quat(1, 0, 0.f, 0.f);
	mState.mScale = glm::vec3(0.f, 0.f, 0.f);
	mPrevState = mState;
	mVisualEntity = e;
}

void DynaObject::update(const float& timeSimulated, const float& period) {
	saveState();
	rotate(glm::vec3(0, 1, 0), glm::radians(45.f) * period);
}

void DynaObject::computeRenderState(const float alpha) {
	mVisualEntity->computeState(mPrevState, mState, alpha);
}

void DynaObject::saveState() {
	mPrevState = mState;
}

void DynaObject::rotate(const glm::vec3& axis, const float& angle) {
	mState.mRot = glm::rotate(mState.mRot, angle, axis);
}

void DynaObject::scale(glm::vec3 scaling) {
	mState.mScale = scaling;
}
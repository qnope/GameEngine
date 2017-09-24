#pragma once
#include <vector>
#include "../Tools/geometry.h"
#include "../SceneGraph/entity.h"



class DynaObject {

public:
	DynaObject(Entity* e);
	void update(const float& timeSimulated, const float& period);
	void saveState();
	void computeRenderState(const float alpha);
	void rotate(const glm::vec3& axis, const float& angle);
	void scale(glm::vec3 scaling);
private:
	State mState;
	State mPrevState;
	Entity *mVisualEntity;
};
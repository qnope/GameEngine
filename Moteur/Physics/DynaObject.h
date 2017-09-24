#pragma once
#include <vector>
#include "../Tools/geometry.h"
#include "../SceneGraph/entity.h"

struct Ref {
	Ref() : mat(glm::mat4{ 1.f }){}
	Ref(glm::mat4 m) : mat(m) {}

	glm::mat3 rot() { return glm::mat3(mat); }
	glm::vec3 pos() { return glm::vec3(mat[3]); }
	glm::mat4 mat;
};

struct PrevState {
	Ref mRef;
	AABB mAABB;
};

class DynaObject {

public:
	DynaObject(Entity* e);
	void saveState();
	void computeRenderState(const float alpha);
	void rotate(const glm::vec3& axis, const float& angle);
private:
	Ref mRef;
	AABB mAABB;
	PrevState mPrevState;
	Entity *mVisualEntity;
};
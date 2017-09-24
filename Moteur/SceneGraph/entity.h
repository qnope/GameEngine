#pragma once

#include "../Tools/glm.h"
#include "../Tools/geometry.h"

struct Ref {
	Ref() : mat(glm::mat4{ 1.f }) {}
	Ref(glm::mat4 m) : mat(m) {}

	glm::mat3 rot() { return glm::mat3(mat); }
	glm::vec3 pos() { return glm::vec3(mat[3]); }
	glm::mat4 mat;
};

struct State {
	Ref mRef;
	AABB mAABB;
};

class Entity {
public:
	Entity(bool *validity, glm::mat4 *matrix, AABB *aabb, const AABB &originalAABB);

	void identity();

	void scale(glm::vec3 scaling);
	void scale(float x, float y, float z);

	void translate(glm::vec3 vec);
	void translate(float x, float y, float z);

	void rotate(glm::mat4 rotatingMatrix);
	void rotate(glm::vec3 axe, float angleInDegree);

	void destroy();

	bool isValid() const;

	void computeState(const State& prevState, const State& state, const float& alpha);

private:
	bool *mValidity;
	glm::mat4 *mMatrix;
	AABB *mAABB;

	const AABB mOriginalAABB;
};

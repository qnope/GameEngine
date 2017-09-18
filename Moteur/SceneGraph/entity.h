#pragma once

#include "../Tools/glm.h"
#include "ModelImporter/mesh.h"

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

private:
	bool *mValidity;
	glm::mat4 *mMatrix;
	AABB *mAABB;

	const AABB mOriginalAABB;
};

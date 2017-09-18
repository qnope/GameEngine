#include "entity.h"

Entity::Entity(bool *validity, glm::mat4 * matrix, AABB *aabb, const AABB &originalAABB) :
	mValidity(validity), mMatrix(matrix), mAABB(aabb), mOriginalAABB(originalAABB)
{
}

void Entity::identity() {
	assert(*mValidity == true);
	*mMatrix = glm::mat4{ 1.f };
	*mAABB = mOriginalAABB * *mMatrix;
}

void Entity::scale(glm::vec3 scaling)
{
	assert(*mValidity == true);
	*mMatrix = glm::scale(*mMatrix, scaling);
	*mAABB = mOriginalAABB * *mMatrix;
}

void Entity::scale(float x, float y, float z)
{
	scale(glm::vec3(x, y, z));
}

void Entity::translate(glm::vec3 vec)
{
	assert(*mValidity == true);
	*mMatrix = glm::translate(*mMatrix, vec);
	*mAABB = mOriginalAABB * *mMatrix;
}

void Entity::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}

void Entity::rotate(glm::mat4 rotatingMatrix)
{
	assert(*mValidity == true);
	*mMatrix *= rotatingMatrix;
	*mAABB = mOriginalAABB * *mMatrix;
}

void Entity::rotate(glm::vec3 axe, float angleInDegree)
{
	assert(*mValidity == true);
	*mMatrix = glm::rotate(*mMatrix, glm::radians(angleInDegree), axe);
	*mAABB = mOriginalAABB * *mMatrix;
}

void Entity::destroy()
{
	*mValidity = false;
}

bool Entity::isValid() const
{
	return mValidity;
}

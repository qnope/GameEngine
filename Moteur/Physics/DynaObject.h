#include <vector>
#include "../Tools/glm.h"
#include "../Tools/Maths/Vec3.h"


void test() {
	//glm::cross()
}



class DynaObject {
	DynaObject();

private:
	std::vector<glm::vec3> mForces;
	std::vector<glm::vec3> mTorques;

	Vec3 mPos;

	glm::vec3 mGravityCenter; // relative to mPos
};
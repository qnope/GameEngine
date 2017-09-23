#include "Mat3.h"
#include <iomanip>
#include <assert.h>



Mat3::Mat3() {
	for (auto v : mLines) {
		v.x = 0;
		v.y = 0;
		v.z = 0;
	}
}


Mat3 Mat3::operator*(const Mat3& mat) const {
	Mat3 result;
	result.mLines[0].set(this->mLines[0].x * mat.mLines[0].x + this->mLines[0].y * mat.mLines[1].x + this->mLines[0].z * mat.mLines[2].x,
		this->mLines[0].x * mat.mLines[0].y + this->mLines[0].y * mat.mLines[1].y + this->mLines[0].z * mat.mLines[2].y,
		this->mLines[0].x * mat.mLines[0].z + this->mLines[0].y * mat.mLines[1].z + this->mLines[0].z * mat.mLines[2].z);

	result.mLines[1].set(this->mLines[1].x * mat.mLines[0].x + this->mLines[1].y * mat.mLines[1].x + this->mLines[1].z * mat.mLines[2].x,
		this->mLines[1].x * mat.mLines[0].y + this->mLines[1].y * mat.mLines[1].y + this->mLines[1].z * mat.mLines[2].y,
		this->mLines[1].x * mat.mLines[0].z + this->mLines[1].y * mat.mLines[1].z + this->mLines[1].z * mat.mLines[2].z);

	result.mLines[2].set(this->mLines[2].x * mat.mLines[0].x + this->mLines[2].y * mat.mLines[1].x + this->mLines[2].z * mat.mLines[2].x,
		this->mLines[2].x * mat.mLines[0].y + this->mLines[2].y * mat.mLines[1].y + this->mLines[2].z * mat.mLines[2].y,
		this->mLines[2].x * mat.mLines[0].z + this->mLines[2].y * mat.mLines[1].z + this->mLines[2].z * mat.mLines[2].z);
	return result;
}

Mat3 Mat3::operator+(const Mat3& mat) const {
	Mat3 result;
	result.mLines[0].set(this->mLines[0] + mat.mLines[0]);
	result.mLines[1].set(this->mLines[1] + mat.mLines[1]);
	result.mLines[2].set(this->mLines[2] + mat.mLines[2]);
	return result;
}


void Mat3::operator*=(const Mat3& mat) {
	*this = *this * mat;
}

void Mat3::operator+=(const Mat3& mat) {
	*this = *this + mat;
}



void Mat3::setRotateX(const float& angle) {
	mLines[0].set(1.f, 0.f, 0.f);
	mLines[1].set(0.f, std::cos(angle), -std::sin(angle));
	mLines[2].set(0.f, std::sin(angle), std::cos(angle));
}

void Mat3::setRotateY(const float& angle) {
	mLines[0].set(std::cos(angle), 0.f, std::sin(angle));
	mLines[1].set(0.f, 1.f, 0.f);
	mLines[2].set(-std::sin(angle), 0.f, std::cos(angle));
}

void Mat3::setRotateZ(const float& angle) {
	mLines[0].set(std::cos(angle), -std::sin(angle), 0.f);
	mLines[1].set(std::sin(angle), std::cos(angle), 0.f);
	mLines[2].set(0.f, 0.f, 1.f);
}

void Mat3::rotateX(const float& angle) {
	Mat3 rot; rot.setRotateX(angle);
	*this *= rot;
}

void Mat3::rotateY(const float& angle) {
	Mat3 rot; rot.setRotateY(angle);
	*this *= rot;
}

void Mat3::rotateZ(const float& angle) {
	Mat3 rot; rot.setRotateZ(angle);
	*this *= rot;
}

void Mat3::setRotate(const float& angle, const Vec3& axis) {
	assert(axis.isNormalized());
	const float cosAngle = std::cos(angle);
	const float sinAngle = std::sin(angle);
	mLines[0].x = cosAngle + (axis.x * axis.x * (1.f - cosAngle));
	mLines[0].y = axis.x * axis.y * (1.f - cosAngle) - axis.z * sinAngle;
	mLines[0].z = axis.x * axis.z * (1.f - cosAngle) + axis.y * sinAngle;

	mLines[1].x = axis.x * axis.y * (1.f - cosAngle) + axis.z * sinAngle;
	mLines[1].y = cosAngle + (axis.y * axis.y * (1.f - cosAngle));
	mLines[1].z = axis.y * axis.z * (1.f - cosAngle) - axis.x * sinAngle;

	mLines[2].x = axis.x * axis.z * (1.f - cosAngle) - axis.y * sinAngle;
	mLines[2].y = axis.y * axis.z * (1.f - cosAngle) + axis.x * sinAngle;
	mLines[2].z = cosAngle + (axis.z * axis.z * (1.f - cosAngle));
}

void Mat3::rotate(const float& angle, const Vec3& axis) {
	assert(axis.isNormalized());
	Mat3 rot; rot.setRotate(angle, axis);
	*this *= rot;
}

Vec3 Mat3::getLine(const unsigned int& nbLine) const {
	return mLines[nbLine];
}

void Mat3::setLine(const unsigned int& nbLine, const float& x, const float& y, const float& z) {
	mLines[nbLine].set(x, y, z);
}

void Mat3::setLine(const unsigned int& nbLine, const Vec3& vec) {
	mLines[nbLine] = vec;
}

void Mat3::set(const Vec3& vec1, const Vec3& vec2, const Vec3& vec3) {
	mLines[0] = vec1;
	mLines[1] = vec2;
	mLines[2] = vec3;
}

void Mat3::getRotation(float& angle, Vec3& axis) {
	// Tag GC: marche pas pour les angles négatifs => c'est l'axe qui s'inverse et pas l'angle
	axis.x = mLines[2].y - mLines[1].z;
	axis.y = mLines[0].z - mLines[2].x;
	axis.z = mLines[1].x - mLines[0].y;
	const float norme = std::sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	float sinTheta = norme / 2.f;
	float cosTheta = (mLines[0].x + mLines[1].y + mLines[2].z - 1) / 2.f;
	if (sinTheta > 0) {
		angle = fabsf(acosf(cosTheta));
	}
	else {
		angle = -fabsf(acosf(cosTheta));
	}
	axis.normalize();
	axis.display();
	std::cout << "angle: " << angle << std::endl;

}




void Mat3::display() {
	for (auto n : mLines) {
		std::cout << std::setprecision(4) << n.x << "\t" << n.y << "\t" << n.z << std::endl;
	}
}



Mat3::~Mat3()
{
}

void Vec3::mult(const Mat3& mat) {
	Vec3 result;
	result.x = x * mat.getLine(0).x + y * mat.getLine(1).x + z * mat.getLine(2).z;
	result.y = x * mat.getLine(0).y + y * mat.getLine(1).y + z * mat.getLine(2).y;
	result.z = x * mat.getLine(0).z + y * mat.getLine(1).z + z * mat.getLine(2).z;

	*this = result;
}

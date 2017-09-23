#pragma once
#include <cmath>
#include <iostream>
#include "Vec3.h"

class Mat3 {
public:
	Mat3();
	~Mat3();

	Mat3 operator*(const Mat3& mat) const;
	Mat3 operator+(const Mat3& mat) const;
	void operator*=(const Mat3& mat);
	void operator+=(const Mat3& mat);

	void display(); // pour débug
	Vec3 getLine(const unsigned int& nbLine) const;
	void setLine(const unsigned int& nbLine, const float& x, const float& y, const float& z);
	void setLine(const unsigned int& nbLine, const Vec3& vec);
	void set(const Vec3& vec1, const Vec3& vec2, const Vec3& vec3);
	void setRotateX(const float& angle);
	void setRotateY(const float& angle);
	void setRotateZ(const float& angle);
	void setRotate(const float& angle, const Vec3& axis);
	void rotateX(const float& angle);
	void rotateY(const float& angle);
	void rotateZ(const float& angle);
	void rotate(const float& angle, const Vec3& axis);
	void getRotation(float& angle, Vec3& axis);
	Vec3 mLines[3];
private:
};

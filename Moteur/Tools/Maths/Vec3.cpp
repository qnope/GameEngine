#include "Vec3.h"
#include <cmath>
#include <iostream>
#include <iomanip>


Vec3::Vec3() : x(0.f), y(0.f), z(0.f)
{}

Vec3::Vec3(const float& x, const float& y, const float& z) {
	this->x = x; this->y = y; this->z = z;
}

Vec3::~Vec3()
{}

void Vec3::display() {
	std::cout << std::setprecision(4) << x << " " << y << " " << z << std::endl;
}

void Vec3::normalize() {
	const float norme = std::sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	this->x /= norme;
	this->y /= norme;
	this->z /= norme;
}

bool Vec3::isNormalized() const {
	return ((this->x * this->x + this->y * this->y + this->z * this->z) == 1.f);
}


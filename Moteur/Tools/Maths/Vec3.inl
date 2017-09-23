#pragma once

inline void Vec3::set(const float& x, const float& y, const float& z) {
	this->x = x;
	this->y = y;
	this->z = z;
}


inline void Vec3::set(const Vec3& vec) {
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
}


inline Vec3 Vec3::operator+(const Vec3& vec) const {
	return Vec3(this->x + vec.x, this->y + vec.y, this->z + vec.z);
}

inline Vec3 Vec3::operator-(const Vec3& vec) const {
	return Vec3(this->x - vec.x, this->y - vec.y, this->z - vec.z);
}

inline void Vec3::operator+=(const Vec3& vec) {
	*this = *this + vec;
}

inline void Vec3::operator-=(const Vec3& vec) {
	*this = *this - vec;
}

inline void Vec3::operator=(const Vec3& vec) {
	x = vec.x; y = vec.y; z = vec.z;
}

inline bool Vec3::operator==(const Vec3& vec) const {
	return (this->x == vec.x && this->y == vec.y && this->z == vec.z);
}
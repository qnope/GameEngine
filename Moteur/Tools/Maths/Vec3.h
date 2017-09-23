#pragma once
class Mat3;
class Vec3
{
public:
	Vec3();
	Vec3(const float& x, const float& y, const float& z);
	~Vec3();
	inline void set(const float& x, const float& y, const float& z);
	inline void set(const Vec3& vec);

	void normalize();
	bool isNormalized() const;
	inline Vec3 operator+(const Vec3& vec) const;
	inline Vec3 operator-(const Vec3& vec) const;
	inline void operator+=(const Vec3& vec);
	inline void operator-=(const Vec3& vec);
	inline void operator=(const Vec3& vec);
	inline bool operator==(const Vec3& vec) const;
	void mult(const Mat3& mat);

	void display();

	float x;
	float y;
	float z;
};
#include "Vec3.inl"

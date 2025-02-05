#pragma once
#include <cmath> // 追加

struct Vector3 final {
	float x, y, z;

	Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }

	Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }

	Vector3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

	Vector3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	void Normalize() {
		float length = std::sqrt(x * x + y * y + z * z);
		if (length != 0) {
			x /= length;
			y /= length;
			z /= length;
		}
	}

	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float Distance(const Vector3& other) const { return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z)); }

};
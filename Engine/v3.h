//
//  v3.h
//  Birds
//
//  Created by Matthew Dillard on 10/15/15.
//  self explainatory
//

#ifndef v3_h
#define v3_h

#include <iostream>
#include <iomanip>
#include <math.h>

#define PI 3.141592653589793f

class v3 {
public:
	float x, y, z;

	v3(const float xx = 0.0, const float yy = 0.0, const float zz = 0.0) : x(xx), y(yy), z(zz) {}

	v3& operator =(const v3& a) {
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	v3& operator +=(const v3& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	v3& operator -=(const v3& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	v3& operator *=(const float& a) {
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	v3& operator /=(const float& a) {
		x /= a;
		y /= a;
		z /= a;
		return *this;
	}

	v3 operator +(const v3& a) const {
		return{ x + a.x, y + a.y, z + a.z };
	}

	v3 operator -(const v3& a) const {
		return{ x - a.x, y - a.y, z - a.z };
	}

	v3 operator *(const v3& a) const {
		return{ x*a.x, y*a.y, z*a.z };
	}

	v3 operator *(const float& a) const {
		return{ x*a, y*a, z*a };
	}

	v3 operator /(const float& a) const {
		return{ x / a, y / a, z / a };
	}

	float abs() const {
		return sqrt(x*x + y*y + z*z);
	}

	v3 normalize() const {
		float len = abs();
		return v3(x / len, y / len, z / len);
	}

	float dot(const v3& a) const {
		return x*a.x + y*a.y + z*a.z;
	}

	float angle(const v3& v_2) const {
		float a = abs();
		float b = v_2.abs();
		return (180.0f / PI) * acos(dot(v_2) / (a * b));
	}

	v3 cross(const v3& a) const {
		return{ y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x };
	}

	bool operator ==(const v3& v) const {
		return (x == v.x && y == v.y && z == v.z);
	}

	v3 rotateX(const float theta) const {
		float costype = cosf(theta * PI / 180.0f);
		float sintype = sinf(theta * PI / 180.0f);
		return{ x, y * costype - z * sintype, y * sintype + z * costype };
	}

	v3 rotateY(const float theta) const {
		float costype = cosf(theta * PI / 180.0f);
		float sintype = sinf(theta * PI / 180.0f);
		return{ x * costype + z * sintype, y, z * costype - x * sintype };
	}

	v3 rotateZ(const float theta) const {
		float costype = cosf(theta * PI / 180.0f);
		float sintype = sinf(theta * PI / 180.0f);
		return{ x * costype - y * sintype, x * sintype + y * costype, z };
	}
};

#endif /* v3_h */

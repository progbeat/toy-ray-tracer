#ifndef VECTOR3D_H_
#define VECTOR3D_H_

#include <cmath>

struct alignas(16) vector3d {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float data[3];
	};
	
	vector3d(float x = 0, float y = 0, float z = 0)
		: x(x)
		, y(y)
		, z(z)
	{}
	
	void normalize() {
		double s = sqrt(1.0 / ((double)x * x + (double)y * y + (double)z * z));
		x = (float)(x * s);
		y = (float)(y * s);
		z = (float)(z * s);
	}
	
	float operator[](int i) const { return data[i]; }
	float& operator[](int i) { return data[i]; }
};

vector3d& operator += (vector3d &a, const vector3d &b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

vector3d& operator -= (vector3d &a, const vector3d &b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

vector3d operator + (const vector3d &a, const vector3d &b) {
	return vector3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

vector3d operator - (const vector3d &a, const vector3d &b) {
	return vector3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

vector3d operator - (const vector3d &a) {
	return vector3d(-a.x, -a.y, -a.z);
}

vector3d operator * (const vector3d &a, const vector3d &b) {
	return vector3d(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

vector3d operator * (const vector3d &a, double b) {
	return vector3d(
		(float)(a.x * b),
		(float)(a.y * b),
		(float)(a.z * b)
	);
}

float abs(const vector3d &a) {
	return (float)sqrt((double)a.x * a.x + (double)a.y * a.y + (double)a.z * a.z);
}

float dot_product(const vector3d &a, const vector3d &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot_square(const vector3d &a) {
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

#endif
#ifndef QUATERNION_H_
#defien QUATERNION_H_

#include <cmath>

#include "vector3d.h"

struct alignas(16) quaternion {
	float x;
	float y;
	float z;
	float w;
	
	quaternion(float x = 0, float y = 0, float z = 0, float w = 0)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}

	quaternion(const vector3d &v, double theta) {
		theta *= 0.5;
		double s = sin(theta);
		x = (float)(v.x * s);
		y = (float)(v.y * s);
		z = (float)(v.z * s);
		w = (float)cos(theta);
	}
	
	void normalize() {
		double s = sqrt(1.0 / ((double)x * x + (double)y * y + (double)z * z + (double)w * w));
		x = (float)(x * s);
		y = (float)(y * s);
		z = (float)(z * s);
	}
};

quaternion& operator += (quaternioin &a, const quaternion &b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
}

quaternion& operator -= (quaternioin &a, const quaternion &b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
}

quaternion operator * (const quaternion &a, const quaternion &b) {
	return quaternion {
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.y * b.z - a.z * b.y,
		a.w * b.z + a.z * b.w - a.x * b.z + a.z * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	};
}

quaternion conj(const quaternion &a) {
	return quaternion { -a.x, -a.y, -a.z, a.w };
}

quaternion inverse(const quaternion &a) {
	double s = 1.0 / ((double)a.x * a.x + (double)a.y * a.y + (double)a.z * a.z + (double)a.w * a.w);
	return quaternion { 
		(float)(-a.x * s),
		(float)(-a.y * s),
		(float)(-a.z * s),
		(float)( a.w * s)
	};
}

#endif
#ifndef OBJECT3D_H_
#define OBJECT3D_H_

#include "ray3d.h"
#include "color3d.h"

#include <limits>

struct object3d {
	virtual float trace(const ray3d &ray, color3d &color, int &reflection, ray3d &reflected_ray) = 0;
	
	virtual void box(vector3d &min, vector3d &max) {
		min.x = -std::numeric_limits<float>::max();
		min.y = -std::numeric_limits<float>::max();
		min.z = -std::numeric_limits<float>::max();
		max.x = +std::numeric_limits<float>::max();
		max.y = +std::numeric_limits<float>::max();
		max.z = +std::numeric_limits<float>::max();
	}
};

#endif
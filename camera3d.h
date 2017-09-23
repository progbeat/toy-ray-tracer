#ifndef CAMERA3D_H_
#define CAMERA3D_H_

#include "vector3d.h"
#include "scene3d.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>

class camera3d {
public:
	vector3d origin;
	float fov;
	int width;
	int height;
	
	camera3d() 
		: xray(1, 0, 0)
		, yray(0, 1, 0)
		, zray(0, 0, 1)
		, fov(120)
		, width(640)
		, height(480)
	{}
	
	void look_at(const vector3d &point) {
		zray = point - origin;
		zray.normalize();
		yray = vector3d(0, 1, 0);
		xray = zray * yray;
		xray.normalize();
		yray = xray * zray;
		yray.normalize();
		const double eps = 1e-7;
		assert(fabs(abs(xray) - 1) < eps);
		assert(fabs(abs(yray) - 1) < eps);
		assert(fabs(abs(zray) - 1) < eps);
		assert(fabs(dot_product(xray, yray)) < eps);
		assert(fabs(dot_product(xray, zray)) < eps);
		assert(fabs(dot_product(yray, zray)) < eps);
	}
	
	void render_to_file(const scene3d &scene, const char *path);
	
private:
	vector3d xray;
	vector3d yray;
	vector3d zray;
};

void camera3d::render_to_file(const scene3d &scene, const char *path) {
	const double pi = 3.1415926535897932384626433832795;
	const double focal_length_inv = 2 * tan(fov / 2) / width;
	std::unique_ptr<uint8_t[]> data(new uint8_t[width * height * 3]);
	vector3d dx = xray * focal_length_inv;
	vector3d dy = yray * focal_length_inv;
	#pragma omp parallel for
	for (int i = 0; i < height; ++i) {
		uint8_t *row = data.get() + i * (width * 3);
		ray3d ray;
		ray.origin = origin;
		vector3d direction = zray + dy * (i - (height - 1) * 0.5) - dx * ((width - 1) * 0.5);
		for (int j = 0; j < width; ++j) {
			ray.direction = direction;
			ray.direction.normalize();
			color3d color = scene.trace(ray);
			int alpha = color.a + 1;
			row[0] = color.r * alpha >> 8;
			row[1] = color.g * alpha >> 8;
			row[2] = color.b * alpha >> 8;
			row += 3;
			direction += dx;
		}
	}
	std::ofstream fout(path, std::ios::out | std::ios::binary);
	fout << "P6" << std::endl;
	fout << width << ' ' << height << std::endl;
	fout << 255 << std::endl;
	fout.write((char*)data.get(), width * height * 3);
}

#endif
#ifndef SCENE3D_H_
#define SCENE3D_H_

#include "object3d.h"

#include <algorithm>
#include <utility>
#include <memory>
#include <vector>

class scene3d {
public:
	typedef std::shared_ptr<object3d> object3d_ptr;
	
	void add(object3d_ptr obj) {
		objects.push_back(obj);
	}
	
	void remove(object3d_ptr obj) {
		for (auto i = objects.begin(); i != objects.end(); ++i) {
			if (i->get() == obj.get()) {
				std::swap(*i, objects.back());
				objects.pop_back();
				return;
			}
		}
	}
	
	color3d trace(const ray3d &ray, int max_depth = 4) const;
private:
	std::vector<object3d_ptr> objects;
	
	static bool less(const std::pair<float, color3d> &lhs, const std::pair<float, color3d> &rhs) {
		return lhs.first < rhs.first;
	}
};

color3d scene3d::trace(const ray3d &ray, int max_depth) const {
	const int TOP = 8;
	std::pair<float, color3d> colors[TOP * 2];
	int num = 0;
	for (auto obj = objects.begin(); obj != objects.end(); ++obj) {
		if (num == TOP * 2) {
			std::nth_element(colors, colors + TOP, colors + num, less);
			num = TOP;
		}
		int reflection = 0;
		ray3d reflected;
		auto &color = colors[num];
		color.first = (*obj)->trace(ray, color.second, reflection, reflected);
		if (color.first <= 1e-9f)
			continue;
		++num;
		if (reflection == 0 || max_depth == 0)
			continue;
		int numLess = 0;
		for (int j = 0; j < num; ++j)
			numLess += (colors[j].first < color.first);
		if (numLess >= TOP) {
			--num;
			continue;
		}
		reflected.origin += reflected.direction * 1.0f;
		auto reflected_color = trace(reflected, max_depth - 1);
		color.second.overlay(reflected_color, reflection);
	}
	if (num > TOP) {
		std::nth_element(colors, colors + TOP, colors + num, less);
		num = TOP;	
	}
	if (num > 0) {
		std::sort(colors, colors + num, less);
		color3d &res = colors[0].second;
		for (int i = 1; i < num && res.a != 255; ++i) {
			res.overlay(colors[i].second);
		}
		return res;
	}
	return color3d{0, 0, 0, 0};
}

#endif
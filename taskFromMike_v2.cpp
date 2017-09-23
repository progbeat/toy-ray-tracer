#include "object3d.h"
#include "camera3d.h"

struct infinite_chessboard : object3d {
	float y;
	float scale;

	static int round(float x) {
		return (int)(x >= 0 ? x + 0.5 : x - 0.5);
	}
	
	infinite_chessboard(float y = 0, float scale = 1)
		: y(y)
		, scale(scale)
	{}

	float trace(const ray3d &ray, color3d &color, int &reflection, ray3d &reflected) {
		float t = (y - ray.origin.y) / ray.direction.y;
		reflected.origin = vector3d(
			ray.origin.x + ray.direction.x * t,
			y,
			ray.origin.z + ray.direction.z * t
		);
		reflected.direction = vector3d(ray.direction.x, -ray.direction.y, ray.direction.z);
		// uint8_t white = 0;
		// if ((round(reflected.origin.x) ^ round(reflected.origin.z)) & 1) {
			// white = (uint8_t)(255.9999 / (1 + 0.05 * t * t));
		// }
		float c = (7 + sinf(reflected.origin.x * scale) * sinf(reflected.origin.z * scale)) * 0.125f;
		color = color3d::from_temperature(c);
		std::swap(color.r, color.g);
		color.a = (uint8_t)(fabs(ray.direction.y) * 255) / 4;
		reflection = 0;
		return t;
	}
};

struct sphere3d : object3d {
	vector3d center;
	float radius;
	int mirror;

	color3d color;
	
	sphere3d() : radius(1), mirror(0) {}
	
	float trace(const ray3d &ray, color3d &color, int &reflection, ray3d &reflected) {
		float b = 0;
		float c = -radius * radius;
		for (int i = 0; i < 3; ++i) {
			float d = ray.origin[i] - center[i];
			b += d * ray.direction[i];
			c += d * d;
		}
		float d = b * b - c;
		if (d <= 0)
			return 0;
		d = sqrtf(d);
		float t = -b - d;
		int sgn = 1;
		if (t <= 0) {
			t += 2 * d;
			sgn = -1;
		}
		if (t <= 0)
			return 0;
		reflected.origin = ray.origin + ray.direction * t;
		auto radius_vector = (center - reflected.origin) * (sgn / radius);
		float ort = dot_product(ray.direction, radius_vector);
		reflected.direction = ray.direction;
		reflected.direction -= radius_vector * (ort * 2);
		color = this->color;
		int mul = (int)(fabs(ort) * 256);
		color.r = color.r * mul >> 8;
		color.g = color.g * mul >> 8;
		color.b = color.b * mul >> 8;
		reflection = mirror;
		return t;
	}
private:
};

vector3d getPointOnCurve(double t) {
	double a = 14 * 3.1415926535897932384626433832795 * t;
	double b = 2 * sqrt(t * (1 - t));
	return vector3d(cos(a) * b, sin(a) * b, 1 - 2 * t);
}

double getAlphaNewthon(const vector3d &v) {
	double t = (1 - v.z) * 0.5;
	const double pi14 = 3.1415926535897932384626433832795 * 14;
	const double pi142 = pi14 * pi14;
	for (int k = 0; k < 8; ++k) {
		const double a = pi14 * t;
		const double bsq = t * (1 - t);
		const double b = sqrt(bsq);
		const double b2 = b * 2;
		const double c = (2 - 4 * t) / b;
		const double d = (2 * t * (1 - 2 * t) + 1) / (b * bsq);
		const double sina = sin(a);
		const double cosa = cos(a);
		const double sina_b2 = sina * b2;
		const double cosa_b2 = cosa * b2;
		const double sina_c = sina * c;
		const double cosa_c = cosa * c;
		double f1 = v.x * (-pi14 * sina_b2 + cosa * c)
		          + v.y * (+pi14 * cosa_b2 + sina * c)
		          - 2 * v.z;
		double f2 = v.x * (-pi142 * cosa_b2 - pi14 * sina_c + cosa * d)
		          + v.y * (+pi142 * sina_b2 + pi14 * cosa_c + sina * d);
		double deltaT = f1 / f2;
		std::cerr << deltaT << ' ';
		t -= deltaT;
		if (t < 0)
			t = 0;
		else if (t > 1)
			t = 1;
	}
	std::cerr << std::endl;
	return t;
}

const float curveRadius = 1.0f / 10000;

float distOnSphere(const vector3d &p, const vector3d &q) {
	return 2 - 2 * dot_product(p, q);
}

float getAlphaStupid(const vector3d &v) {
	const float a = (float)(14 * 3.1415926535897932384626433832795);
	float t0 = (1 - v.z) * 0.5;
	vector3d p0 = getPointOnCurve(t0);
	float d0 = distOnSphere(p0, v);
	// if (d0 > curveRadius * 1000)
		// return -1;	
	float t1 = std::max(t0 - curveRadius * 0.5f, 0.0f);
	float t2 = std::min(t0 + curveRadius * 0.5f, 1.0f);
	vector3d p1 = getPointOnCurve(t1);
	vector3d p2 = getPointOnCurve(t2);
	float d1 = distOnSphere(p1, v);
	float d2 = distOnSphere(p2, v);
	//std::cerr << dot_square(p2) << ' ' << d0 << ' ' << d1 << ' ' << d2 << std::endl;
	return d1 <= d2
		? t0 - ((d0 - d1) / distOnSphere(p0, p1) + 1) * 0.5f * (t1 - t0)
		: t0 + ((d0 - d2) / distOnSphere(p0, p2) + 1) * 0.5f * (t2 - t0);
}

struct mikes_curve : object3d {
	float trace(const ray3d &ray, color3d &color, int &reflection, ray3d &reflected) {
		float b = 0;
		float c = -1;
		for (int i = 0; i < 3; ++i) {
			float d = ray.origin[i];
			b += d * ray.direction[i];
			c += d * d;
		}
		float d = b * b - c;
		if (d <= 0)
			return 0;
		d = sqrtf(d);
		float t = -b - d;
		int sgn = 1;
		for (int sgn = 1; sgn >= -1; sgn -= 2, t += 2 * d) {
			if (t <= 0)
				continue;
			reflected.origin = ray.origin + ray.direction * t;
			float alpha = getAlphaStupid(reflected.origin);
			if (alpha < 0 || alpha > 1)
				continue;
			auto poc = getPointOnCurve(alpha);
			float dist2 = dot_square(poc - reflected.origin);
			if (dist2 > curveRadius)
				continue;
			auto radius_vector = (-reflected.origin) * sgn;
			float ort = dot_product(ray.direction, radius_vector);
			reflected.direction = ray.direction;
			reflected.direction -= radius_vector * (ort * 2);
			color.r = 255;
			color.g = 255;
			color.b = 255;
			color.a = 255;
			reflection = 0;
			reflected.origin = reflected.origin;
			return t * (1 - sgn * 0.001f);
		}
		return 0;
	}
};

vector3d getPointOnCurveOld(double alpha) {
	double a = alpha * 3.1415926535897932384626433832795 * 7 / 6;
	double b = 2 * sqrt(alpha * (1 - alpha));
	return vector3d(cos(a) * b, sin(a) * b, 1 - 2 * alpha);
}

int main() {
	scene3d scene;
	scene.add(std::make_shared<infinite_chessboard>(-10, 1.0 / 2));
	auto sphere = std::make_shared<sphere3d>();
	sphere->radius = 1;
	sphere->color = color3d{0, 255, 255, 223};
	sphere->mirror = 0;
	scene.add(sphere);
	scene.add(std::make_shared<mikes_curve>());
	// sphere = std::make_shared<sphere3d>();
	// sphere->radius = 50;
	// sphere->color = color3d{255, 0, 255, 127};
	// sphere->center = vector3d(0, -10, 0);
	// sphere->mirror = 0;
	// scene.add(sphere);
	scene.add(std::make_shared<mikes_curve>());
	for (int i = 0; i <= 12; ++i) {
		sphere = std::make_shared<sphere3d>();
		sphere->radius = 0.02;
		sphere->color = color3d{0, 0, 255, 255};
		sphere->center = getPointOnCurve(i * (1.0 / 12));
		sphere->mirror = 0;
		scene.add(sphere);
	}
	//scene.add(std::make_shared<infinite_chessboard>(-20));
	//scene.add(std::make_shared<infinite_chessboard>(20));
	camera3d camera;
	camera.fov = 130;
	camera.width = 1280;
	camera.height = 720;
	const int n = 180;
	const double rs[] = {1.66, 1.66, -0.9};
	const double pi = acos(-1.0);
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < n; ++i) {
			auto point = getPointOnCurveOld(i * (1.0 / n));
			if (j == 0) {
				double angle = 2 * pi * i / n;
				camera.origin = vector3d(sin(angle) * rs[j], 0, cos(angle) * rs[j]);
				camera.look_at(vector3d());
			} else {
				camera.origin = point * rs[j];
				camera.look_at(point);	
			}
			char filename[256];
			sprintf(filename, "mike2/frame%004d.ppm", j * n + i);
			camera.render_to_file(scene, filename);
			std::cout << j * n + i + 1 << std::endl;
		}
	}
	return 0;
}
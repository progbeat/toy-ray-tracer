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
		color.a = (uint8_t)(fabs(ray.direction.y) * 255);
		reflection = 191;
		return t;
	}
};

struct sphere3d : object3d {
	vector3d center;
	float radius;

	color3d color;
	
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
		color = color3d::from_temperature(cos(reflected.origin.x));
		//std::swap(color.g, color.b);
		color.a = 127;
		color.overlay(this->color);
		int mul = (int)(fabs(ort) * 256);
		color.r = color.r * mul >> 8;
		color.g = color.g * mul >> 8;
		color.b = color.b * mul >> 8;
		color.a = 191;
		reflection = 127;
		return t;
	}
private:
};

int main() {
	scene3d scene;
	scene.add(std::make_shared<infinite_chessboard>(-10, 1.0 / 2));
	scene.add(std::make_shared<infinite_chessboard>(10, 2));
	auto sphere = std::make_shared<sphere3d>();
	sphere->radius = 20;
	sphere->color = color3d{0, 0, 255, 127};
	scene.add(sphere);
	//scene.add(std::make_shared<infinite_chessboard>(-20));
	//scene.add(std::make_shared<infinite_chessboard>(20));
	camera3d camera;
	camera.fov = 130;
	camera.width = 1280;
	camera.height = 720;
	const int n = 300;
	const int r0 = 10;
	const int r1 = 30;
	const double pi = acos(-1.0);
	for (int i = 0; i < n; ++i) {
		double angle = i * 4 * (pi / n);
		camera.origin = vector3d(r0 * cos(angle), sin(angle / 2) * sqrt(angle) * 9, r1 * sin(angle));
		camera.look_at(vector3d());	
		char filename[256];
		sprintf(filename, "out/frame%004d.ppm", i);
		camera.render_to_file(scene, filename);
		std::cout << i + 1 << std::endl;
	}
	return 0;
}
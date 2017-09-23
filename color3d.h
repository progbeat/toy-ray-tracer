#ifndef COLOR3D_H_
#define COLOR3D_H_

#include <cstdint>

struct color3d {
	union {
		struct {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		uint8_t channels[4];
	};
	
	uint8_t& operator[](int i) { return channels[i]; }
	uint8_t operator[](int i) const { return channels[i]; }
	
	void overlay(const color3d &under) {
		const int alpha = (a + 1) << 8;
		const int beta = (256 - a) * (under.a + 1);
		r = (uint8_t)((r * alpha + under.r * beta) >> 16);
		g = (uint8_t)((g * alpha + under.g * beta) >> 16);
		b = (uint8_t)((b * alpha + under.b * beta) >> 16);
		a = (uint8_t)((65536 - (256 - a) * (256 - under.a)) >> 8);
	}
	
	void overlay(const color3d &under, int reflection) {
		const int alpha = (256 - reflection) << 8;
		const int beta = reflection * (under.a + 1);
		r = (uint8_t)((r * alpha + under.r * beta) >> 16);
		g = (uint8_t)((g * alpha + under.g * beta) >> 16);
		b = (uint8_t)((b * alpha + under.b * beta) >> 16);
	}
	
	static color3d from_temperature(float t) {
		int r = (int)(384 - fabs(0.75f - t) * 1024);
        int g = (int)(384 - fabs(0.50f - t) * 1024);
        int b = (int)(384 - fabs(0.25f - t) * 1024);
        r = r < 0 ? 0 : r < 256 ? r : 255;
        g = g < 0 ? 0 : g < 256 ? g : 255;
        b = b < 0 ? 0 : b < 256 ? b : 255;
        color3d color;
		color.r = (uint8_t)r;
		color.g = (uint8_t)g;
		color.b = (uint8_t)b;
		return color;
	}
};

#endif
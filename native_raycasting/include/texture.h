#pragma once

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct Texture {
	int width;
	int height;
	struct Color **pixels;
};

void textureFromPixels(char* pixels, struct Texture* texture, int channels);
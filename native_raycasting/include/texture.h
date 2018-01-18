#pragma once
#include <dmsdk/sdk.h>
union Color {
	uint8_t colors[4];
	int rgba;
};

struct Texture {
	int width;
	int height;
	Color **pixels;
};

void textureFromPixels(char* pixels, struct Texture* texture, int channels);
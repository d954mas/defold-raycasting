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

struct Sprite
{
	double x;
	double y;
	int textureId;
	double dx;
	double dy;
};

void textureFromPixels(char* pixels, struct Texture* texture, int channels);
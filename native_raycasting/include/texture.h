#pragma once
#include <dmsdk/sdk.h>
struct Color {
	int r;
	int g;
	int b;
	int a;
};

struct Texture {
	int width;
	int height;
	struct Color **pixels;
};

void textureFromPixels(char* pixels, struct Texture* texture, int channels);
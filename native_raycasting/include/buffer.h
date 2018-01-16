#pragma once

#include <dmsdk/sdk.h>
#include "texture.h"
struct Buffer {
	int width;
	int height;
	uint8_t* stream;
};


void initBuffer(struct Buffer*, int, int, dmScript::LuaHBuffer*);
void clearBuffer(struct Buffer*);
void setPixel(struct Buffer*, int, int, Color*);

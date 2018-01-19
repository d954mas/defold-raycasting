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
static inline void setPixel(struct Buffer* buffer, int x, int y, Color *color){
	int id = (y * buffer->width + x) * 3;
	uint8_t* stream = buffer->stream;
	uint8_t* colors = color->colors;
	stream[id] = colors[0];
	stream[id + 1] = colors[1];
	stream[id + 2] = colors[2];
}
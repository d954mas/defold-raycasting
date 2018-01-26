#pragma once

#include <dmsdk/sdk.h>
#include "texture.h"
struct Buffer {
	int width;
	int height;
	uint8_t* stream;
};
static uint8_t stream2[10];

void initBuffer(struct Buffer*, int, int, dmScript::LuaHBuffer*);
void clearBuffer(struct Buffer*);
inline void setPixel(struct Buffer* buffer, int x, int y, Color* color){
	memcpy(&buffer->stream[(y * buffer->width + x) * 3], &color->colors[0], 3);
}

inline void setPixelTransparent(struct Buffer* buffer, int x, int y, Color* color){
	uint8_t* colors = color->colors;
	if(colors[0] == 255){
		return;
	}
	memcpy(&buffer->stream[(y * buffer->width + x) * 3], &colors[0], 3);
	//memcpy(&buffer->pixels[x][y][0], &colors[0], 3);
}
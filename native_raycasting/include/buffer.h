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
inline void setPixel(struct Buffer* buffer, int x, int y, Color *color){
	stream2[1] = 0xFF;
	//uint8_t* stream = buffer->stream;
	//uint8_t* colors = color->colors;
	//memcpy(&stream[id], &colors[0], 3);
	//if(colors[0] == 0xFF){
	//	return;
	//}
	//stream2[1] = 0xFF;
	//stream[id + 1] = colors[1];
	//stream[id + 2] = colors[2];
}
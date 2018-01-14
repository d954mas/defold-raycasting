#ifndef BUFFER
#define BUFFER

#include <dmsdk/sdk.h>
#include "world_structures.h"
struct Buffer {
	int width;
	int height;
	uint8_t* stream;
};


void initBuffer(struct Buffer*, int, int, dmScript::LuaHBuffer*);
void clearBuffer(struct Buffer*);
void setPixel(struct Buffer*, int, int, Color*);

#endif
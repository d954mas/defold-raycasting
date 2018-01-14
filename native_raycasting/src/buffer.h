#ifndef BUFFER
#define BUFFER

#include <dmsdk/sdk.h>
struct Buffer {
	int width;
	int height;
	uint8_t* stream;
};


void initBuffer(struct Buffer*, int, int, dmScript::LuaHBuffer*);
//struct Map parseMap(lua_State*);

#endif
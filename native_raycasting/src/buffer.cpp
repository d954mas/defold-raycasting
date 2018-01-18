#include <dmsdk/sdk.h>
#include "buffer.h"
#include "world_structures.h"

void initBuffer(struct Buffer* buffer, int width, int height, dmScript::LuaHBuffer* luaBuffer){
	buffer->width = width;
	buffer->height = height;
	dmBuffer::HBuffer hBuffer = luaBuffer->m_Buffer;
	uint32_t size_ignored = 0;
	uint32_t components = 0;
	uint32_t stride = 0;
	dmBuffer::Result r = dmBuffer::GetStream(hBuffer, dmHashString64("rgb"), (void**)&buffer->stream, &size_ignored, &components, &stride);
}

void setPixel(struct Buffer* buffer, int x, int y, Color *color){
	int id = (y * buffer->width + x) * 3;
	buffer->stream[id] = color->colors[0];
	buffer->stream[id + 1] = color->colors[1];
	buffer->stream[id + 2] = color->colors[2];
}
void clearBuffer(struct Buffer* buffer){
	for(int i=0; i< (buffer->width * buffer->height * 3); i++){
		buffer->stream[i] = 0;
	}
}
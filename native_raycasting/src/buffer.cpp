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
	dmBuffer::Result r = dmBuffer::GetBytes(hBuffer, (void**)&buffer->stream, &size_ignored);
	//dmBuffer::Result r = dmBuffer::GetStream(hBuffer, dmHashString64("rgb"),(void**)&buffer->stream , &size_ignored, &components, &stride);
}

void clearBuffer(struct Buffer* buffer){
	int size = buffer->width * buffer->height * 3;
	for(int i=0; i< size; i++){
		buffer->stream[i] = 0;
	}
}
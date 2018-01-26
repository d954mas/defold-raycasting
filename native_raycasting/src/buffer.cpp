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
	/*uint8_t *** pixels = (uint8_t ***)malloc(width * sizeof(uint8_t*));

	for(int x = 0; x<width; x++){
		pixels[x] = (uint8_t **)malloc(height * sizeof(uint8_t*));
		for(int y = 0; y<height; y++){
			pixels[x][y] = &buffer->stream[(y * width + x) * 3];
		}
	}
	buffer->pixels = pixels;*/
	//dmBuffer::Result r = dmBuffer::GetStream(hBuffer, dmHashString64("rgb"),(void**)&buffer->stream , &size_ignored, &components, &stride);
}

void clearBuffer(struct Buffer* buffer){
	int size = buffer->width * buffer->height * 3;
	for(int i=0; i< size; i++){
		buffer->stream[i] = 0;
	}
}
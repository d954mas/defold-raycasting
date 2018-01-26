#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "png.h"
#include "arrays.h"
void decodeToTexture(dmBuffer::HBuffer hBuffer, struct Texture* texture){
	uint32_t outW = 0;
	uint32_t outH = 0;
	unsigned char* decodePixels = decodeBuffer(hBuffer, &outW, &outH); 
	printf("texture width:%d height:%d\n", outW, outH);
	clearArrayColor(texture->pixels, texture->height);
	texture->width = outW;
	texture->height = outH;
	texture->heightM = outH-1;
	texture->widthM = outW-1;
	Color **pixels = createTwoArrayColor(outW, outH);
	texture->pixels = pixels;
	int id = 0;
	for(int y=0; y < outH; y++){
		for(int x=0; x < outW; x++){
			Color* color = &pixels[x][outH - y - 1];
			//printf("y:%d x:%d",y,x);
			color->colors[0] = decodePixels[id*4];
			color->colors[1] = decodePixels[id*4+1];
			color->colors[2] = decodePixels[id*4+2];
			color->colors[3] = decodePixels[id*4+3];
			id++;
		}
	}
}

unsigned char* decodeBuffer(dmBuffer::HBuffer hBuffer, uint32_t* pngW, uint32_t* pngH){
	char* data = 0;
	uint32_t datasize = 0;
	dmBuffer::GetBytes(hBuffer, (void**)&data, &datasize);
	return decodeChar(data, datasize, pngW, pngH);
}
/*
get char array from png file bytes
*/
unsigned char* decodeChar(char* png, uint32_t pngLen, uint32_t* pngW, uint32_t* pngH){
    // decode png to pixels
    unsigned char* pixels = 0;
    uint32_t outw = 0;
    uint32_t outh = 0;
    lodepng::State state;
    state.info_raw.bitdepth = 8;
    state.info_raw.colortype = LCT_RGBA;
	lodepng_decode(&pixels, &outw, &outh, &state, (unsigned char*)png, pngLen);
	*pngW = outw;
	*pngH = outh;
	return pixels;
}
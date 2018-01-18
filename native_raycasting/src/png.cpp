#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "png.h"
#include "arrays.h"
void decodeToTexture(dmBuffer::HBuffer hBuffer, int channels, struct Texture* texture){
	uint32_t outW = 0;
	uint32_t outH = 0;
	unsigned char* decodePixels = decodeBuffer(hBuffer, channels, &outW, &outH); 
	printf("texture width:%d height:%d\n", outW, outH);
	clearArrayColor(texture->pixels, texture->height);
	texture->width = outW;
	texture->height = outH;
	struct Color **pixels = createTwoArrayColor(outW, outH);
	texture->pixels = pixels;
	int id = 0;
	for(int y=0; y < outH; y++){
		for(int x=0; x < outW; x++){
			struct Color* color = &pixels[outH - y - 1][x];
			//printf("y:%d x:%d",y,x);
			color->r = decodePixels[id*4];
			color->g = decodePixels[id*4+1];
			color->b = decodePixels[id*4+2];
			id++;
		}
	}
}

unsigned char* decodeBuffer(dmBuffer::HBuffer hBuffer, int channels, uint32_t* pngW, uint32_t* pngH){
	char* data = 0;
	uint32_t datasize = 0;
	dmBuffer::GetBytes(hBuffer, (void**)&data, &datasize);
	return decodeChar(data, datasize, channels, pngW, pngH);
}
/*
get char array from png file bytes
*/
unsigned char* decodeChar(char* png, uint32_t pngLen, int channels, uint32_t* pngW, uint32_t* pngH){
    // decode png to pixels
    unsigned char* pixels = 0;
    uint32_t outw = 0;
    uint32_t outh = 0;
    uint32_t bytes_per_pixel = channels;
    lodepng::State state;
	state.decoder.color_convert = 1;
    state.info_raw.bitdepth = 8;
	switch(channels) {
        case 3:
            state.info_raw.colortype = LCT_RGBA;
            break;
		case 4:
			state.info_raw.colortype = LCT_RGBA;
			break;
        default:
            break;
    }
	lodepng_decode(&pixels, &outw, &outh, &state, (unsigned char*)png, pngLen);
    // flip vertically
    //for (int yi=0; yi < (outh / 2); yi++) {
      //  for (int xi=0; xi < outw; xi++) {
         //   unsigned int offset1 = (xi + (yi * outw)) * bytes_per_pixel;
           // unsigned int offset2 = (xi + ((outh - 1 - yi) * outw)) * bytes_per_pixel;
        //    for (int bi=0; bi < bytes_per_pixel; bi++) {
             //   unsigned char byte1 = pixels[offset1 + bi];
             //   unsigned char byte2 = pixels[offset2 + bi];
               // pixels[offset1 + bi] = byte2;
              //  pixels[offset2 + bi] = byte1;
          //  }
       //}
	//}
	*pngW = outw;
	*pngH = outh;
	return pixels;
}
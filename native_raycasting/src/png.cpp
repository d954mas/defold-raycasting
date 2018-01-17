#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "png.h"

void decodeToTexture(dmBuffer::HBuffer hBuffer, int width, int height, int channels, struct Texture* texture){

}

unsigned char* decodeBuffer(dmBuffer::HBuffer hBuffer, int width, int height, LodePNGColorType type){
	char* data = 0;
	uint32_t datasize = 0;
	dmBuffer::GetBytes(hBuffer, (void**)&data, &datasize);
	return decodeChar(data, width, height, type);
}
/*
get char array from png file bytes
*/
unsigned char* decodeChar(char* png, int width, int height, LodePNGColorType type) {
    // decode png to pixels
	size_t png_length;
    unsigned char* pixels = 0;
    uint32_t outw = 0;
    uint32_t outh = 0;
    uint32_t bytes_per_pixel;
    lodepng::State state;
    state.decoder.color_convert = 1;
    state.info_raw.bitdepth = 8;
    switch(type) {
        case LCT_RGBA:
            state.info_raw.colortype = LCT_RGBA;
            bytes_per_pixel = 4;
            break;
        case LCT_RGB:
        default:
            state.info_raw.colortype = LCT_RGB;
            bytes_per_pixel = 3;
            break;
    }
    lodepng_decode(&pixels, &outw, &outh, &state, (unsigned char*)png, png_length);
    // flip vertically
    for (int yi=0; yi < (outh / 2); yi++) {
        for (int xi=0; xi < outw; xi++) {
            unsigned int offset1 = (xi + (yi * outw)) * bytes_per_pixel;
            unsigned int offset2 = (xi + ((outh - 1 - yi) * outw)) * bytes_per_pixel;
            for (int bi=0; bi < bytes_per_pixel; bi++) {
                unsigned char byte1 = pixels[offset1 + bi];
                unsigned char byte2 = pixels[offset2 + bi];
                pixels[offset1 + bi] = byte2;
                pixels[offset2 + bi] = byte1;
            }
        }
    }
	return pixels;
}
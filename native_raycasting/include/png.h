#pragma once

#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "texture.h"

/*
read png from char array or from defold buffer
return char array for pixels
*/

unsigned char* decodeBuffer(dmBuffer::HBuffer, int, uint32_t*, uint32_t*);
unsigned char* decodeChar(char*,uint32_t, int, uint32_t*, uint32_t*);
void decodeToTexture(dmBuffer::HBuffer, int, struct Texture*);
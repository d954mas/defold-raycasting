#pragma once

#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "texture.h"

/*
read png from char array or from defold buffer
return char array for pixels
*/

unsigned char* decodeBuffer(dmBuffer::HBuffer, int, int, LodePNGColorType);
unsigned char* decodeChar(char*, int, int, LodePNGColorType);
void decodeToTexture(dmBuffer::HBuffer, int, int, int, struct Texture*);
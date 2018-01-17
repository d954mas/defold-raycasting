#pragma once

#include <dmsdk/sdk.h>
#include "lodepng.h"
#include "texture.h"

/*
read png from char array or from defold buffer
return char array for pixels
*/

unsigned char* decodeBuffer(dmBuffer::HBuffer, int);
unsigned char* decodeChar(char*, int);
void decodeToTexture(dmBuffer::HBuffer, int, struct Texture*);
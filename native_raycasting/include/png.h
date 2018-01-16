#pragma once

#include <dmsdk/sdk.h>
#include "lodepng.h"

/*
read png from char array or from defold buffer
return char array for pixels
*/

unsigned char* decodeBuffer(dmBuffer::HBuffer, int, int, LodePNGColorType);
unsigned char* decodeChar(char*, int, int, LodePNGColorType);
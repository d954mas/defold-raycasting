#include <dmsdk/sdk.h>
#include "world_structures.h"
#include "buffer.h"
#include "raycasting.h"

static struct Camera cameras[1];
static struct Buffer buffer;
static struct Map map;
void updateCamera(int cameraId, double x, double y, double angle){
	updateCamera(&cameras[cameraId], x, y, angle);
}

void setBuffer(int width, int height,dmScript::LuaHBuffer* luaBuffer){
	initBuffer(&buffer, width, height, luaBuffer);
}

void setMap(lua_State* L){
	parseMap(L, &map);
}

void castSingleRay(int cameraId, double rayAngle, double *perpDist, double *catetX, 
	double *catetY, int *mapXResult, int *mapYResult, double *textureX){
		castRay(&cameras[cameraId], map.walls, rayAngle, perpDist, catetX, 
			catetY, mapXResult, mapYResult, textureX);
}
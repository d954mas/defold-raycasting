#include <dmsdk/sdk.h>
#include "world_structures.h"
#include "buffer.h"
#include "raycasting.h"

static struct Camera camera;
static struct Buffer buffer;
static struct Map map;
static struct Plane plane;
void updateCamera(double x, double y, double angle){
	updateCamera(&camera, x, y, angle);
}

void setBuffer(int width, int height,dmScript::LuaHBuffer* luaBuffer){
	initBuffer(&buffer, width, height, luaBuffer);
}

void updatePlane(int x, int y, int endX, int endY){
	updatePlane(&plane, x, y, endX, endY);
}

void setMap(lua_State* L){
	parseMap(L, &map);
}

void castSingleRay(double rayAngle, double *perpDist, double *catetX, 
	double *catetY, int *mapXResult, int *mapYResult, double *textureX){
		castRay(&camera, map.walls, rayAngle, perpDist, catetX, 
			catetY, mapXResult, mapYResult, textureX);
}
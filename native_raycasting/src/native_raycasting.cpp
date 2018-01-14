#include <dmsdk/sdk.h>
#include <math.h>
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

void setCameraFov(double fov){
	setCameraFov(&camera, fov);
}

void setBuffer(int width, int height, dmScript::LuaHBuffer* luaBuffer){
	initBuffer(&buffer, width, height, luaBuffer);
}

void clearBuffer(){
	clearBuffer(&buffer);
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

void castRays(){
	double currentAngle = - camera.fov / 2.0;
	double rayAngle = camera.fov/plane.width;
	double perpDist, catetX, catetY, textureX;
	int mapX, mapY;
	int halfPlaneHeight = plane.height/2;
	for (int x = 0; x < plane.width; x++){
		castSingleRay(currentAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
		double endPositionX = camera.x + catetX;
		double endPositionY = camera.y + catetY;
		currentAngle += rayAngle;
		//fix fov and aspect here
		int lineHeight = round(plane.height / perpDist);
		if(lineHeight < 2){
			lineHeight = 2;
		}else if(lineHeight > plane.height){
			lineHeight = plane.height;
		}
		int halfLineHeight = (int)(lineHeight/2.0 + 0.5);
		int drawStart = halfPlaneHeight - halfLineHeight;
		int drawEnd =  halfLineHeight + halfPlaneHeight;
		//draw vert line
		//int textureId = map.walls[mapY][mapX]-1;
		//Texture *wall = &wallTextures[textureId];
	//	int pixelX = (int)((wall->width-1) * textureX);
	//	double wallHeight = halfLineHeight /31.5; //lineHeight /63
	//	double pixelY = 0;
		//double pixelYAdd = 1.0 / wallHeight;
		struct Color color;
		color.r = 255;
		color.b = 0;
		color.g = 0;
		for (int y = drawStart; y <= drawEnd; y++) {
			setPixel(&buffer, x, y, &color);
		}
	}
}
#include <dmsdk/sdk.h>
#include <math.h>
#include "world_structures.h"
#include "buffer.h"
#include "raycasting.h"
#include "texture.h"
#include "png.h"

static struct Camera camera;
static struct Buffer buffer;
static struct Map map;
static struct Plane plane;
static struct Texture textures[10];

void loadTexture(dmScript::LuaHBuffer* luaBuffer, int id){
	decodeToTexture(luaBuffer->m_Buffer, &textures[id]);
}

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
	int halfPlaneHeight = plane.height >> 1;
	for (int x = 0; x < plane.width; x++){
		castSingleRay(currentAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
		double endPositionX = camera.x + catetX;
		double endPositionY = camera.y + catetY;
		currentAngle += rayAngle;
		//fix fov and aspect here
		//height is always even
		
		int lineHeight = ((int)round(plane.height / perpDist)) & 0xFFFFFFFE;
		if(lineHeight > plane.height){
			lineHeight = plane.height;
		}
		
		int halfLineHeight = lineHeight>>1;
		int drawStart = halfPlaneHeight - halfLineHeight;
		int drawEnd =  halfLineHeight + halfPlaneHeight;
		//draw vert line
		int textureId = map.walls[mapY][mapX];
		Texture* texture = &textures[textureId];
		Color** pixels = texture->pixels;
		//	printf("textureId:%d", textureId);
		int textureWidth = texture->width-1;
		int pixelX = (int)( textureWidth* textureX);
		double wallHeight = (double)lineHeight /textureWidth; //lineHeight /63
		double pixelY = 0;
		double pixelYAdd = 1.0 / wallHeight;
		
		for (int y = drawStart; y <= drawEnd; y++) {
		//	setPixel(&buffer, x, y, &pixels[(int)pixelY][pixelX]);
			pixelY += pixelYAdd;
		}
		//draw floor and ceilings
		double n;
		for(int y = 0; y < drawStart; y++){
			double currentDist = (double)halfPlaneHeight / (halfPlaneHeight - y);
			double weight = currentDist / perpDist;
			double weight2 = 1.0 - weight;
			double floorX = (weight * endPositionX + weight2 * camera.x);
			double floorY = (weight * endPositionY + weight2 * camera.y);
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);
			int floorId = map.floors[cellY][cellX];
			int ceilId = map.ceils[cellY][cellX];
			Texture* floorTexture = &textures[floorId];
			int textureX = (int)(modf(floorX,&n) * floorTexture->width);
			int textureY = (int)(modf(floorY,&n) * floorTexture->height);
			Color *color = &(floorTexture->pixels[textureY][textureX]);
			//setPixel(&buffer, x, y, color);

			floorTexture = &textures[ceilId];
			textureX = (int)(modf(floorX,&n) * floorTexture->width);
			textureY = (int)(modf(floorY,&n) * floorTexture->height);
			color = &(floorTexture->pixels[textureY][textureX]);
			//setPixel(&buffer, x, plane.height-y-1, color);
		}
	}
}
#define TWO_PI       3.14159265358979323846 * 2

#include <dmsdk/sdk.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
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
static struct Texture spritesTextures[10];
static double *PRE_CALC_HEIGHT_DISTANCE;
static double *distanceBuffer;
std::vector<Sprite> sprites;
void addSprite(double x, double y, int textureId){
	Sprite sprite = {x,y,textureId,0};
	sprites.push_back(sprite); 
}

void loadTexture(dmScript::LuaHBuffer* luaBuffer, int id){
	decodeToTexture(luaBuffer->m_Buffer, &textures[id]);
}

void loadSprite(dmScript::LuaHBuffer* luaBuffer, int id){
	decodeToTexture(luaBuffer->m_Buffer, &spritesTextures[id]);
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
	free(PRE_CALC_HEIGHT_DISTANCE);
	double size = plane.height>>1;
	PRE_CALC_HEIGHT_DISTANCE = (double *)malloc(sizeof(double) * size);
	for(int i = 0;  i < size; i++){
		PRE_CALC_HEIGHT_DISTANCE[i] =  size / (size - i);
	}
	free(distanceBuffer);
	distanceBuffer = (double *)malloc(sizeof(double) * plane.width);
}

void setMap(lua_State* L){
	parseMap(L, &map);
}

void castSingleRay(double rayAngle, double *perpDist, double *catetX, 
	double *catetY, int *mapXResult, int *mapYResult, double *textureX){
		castRay(&camera, map.walls, rayAngle, perpDist, catetX, 
			catetY, mapXResult, mapYResult, textureX);
}

static bool sortSprites(const Sprite &a, const Sprite &b)
{
	return a.distance < b.distance;
}

static inline void countVertY(double dist, int* height, int* drawStart, int* drawEnd){
	int lineHeight = ((int)round(plane.height / dist)) & 0xFFFFFFFE;
	if(lineHeight > plane.height){
		lineHeight = plane.height;
	}
	int halfLineHeight = lineHeight>>1;
	*drawStart = plane.halfHeight - halfLineHeight;
	*drawEnd =  plane.halfHeight + halfLineHeight;
	*height = lineHeight;
}

static inline void drawVertLine(int x, int drawStart, int drawEnd, double pixelYAdd, Color** pixels, int pixelX){
	double pixelY = 0;
	for (int y = drawStart; y < drawEnd; y++) {
		setPixel(&buffer, x, y, &pixels[(int)pixelY][pixelX]);
		pixelY += pixelYAdd;
	}
}

void castRays(){
	double currentAngle = - camera.fov / 2.0;
	double rayAngle = camera.fov/plane.width;
	double perpDist, catetX, catetY, textureX;
	int mapX, mapY;
	for (int x = 0; x < plane.width; x++){
		castSingleRay(currentAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
		distanceBuffer[x] = perpDist;
		double endPositionX = camera.x + catetX;
		double endPositionY = camera.y + catetY;
		currentAngle += rayAngle;
		//fix fov and aspect here
		//height is always even
		int lineHeight, drawStart,drawEnd;
		countVertY(perpDist,&lineHeight,&drawStart,&drawEnd);
		int textureId = map.walls[mapY][mapX];
		Texture* texture = &textures[textureId];
		int pixelX = (int)( (texture->width-1)* textureX);
		double pixelYadd = (texture->height-1)/(double) lineHeight;
		drawVertLine(x, drawStart, drawEnd, pixelYadd, texture->pixels, pixelX);
		
		//draw floor and ceilings
		double n;
		int** floors =  map.floors;
		int** ceils =  map.ceils;
		for(int y = 0; y < drawStart; y++){
			double currentDist = PRE_CALC_HEIGHT_DISTANCE[y];
			double weight = currentDist / perpDist;
			double weight2 = 1.0 - weight;
			double floorX = (weight * endPositionX + weight2 * camera.x);
			double floorY = (weight * endPositionY + weight2 * camera.y);
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);
			int floorId = floors[cellY][cellX];
			int ceilId = ceils[cellY][cellX];
			Texture* floorTexture = &textures[floorId];
			int textureX = (floorX - (int)floorX) * floorTexture->width;
			int textureY = (floorY - (int)floorY) * floorTexture->height;
			Color *color = &(floorTexture->pixels[textureY][textureX]);
			setPixel(&buffer, x, y, color);

			floorTexture = &textures[ceilId];
			//floor and ceil use same width
		//	textureX = (int)(modf(floorX,&n) * floorTexture->width);
		//	textureY = (int)(modf(floorY,&n) * floorTexture->height);
			color = &(floorTexture->pixels[textureY][textureX]);
			setPixel(&buffer, x, plane.height-y-1, color);
		}
	}
	//draw sprites
	//add 0.5 to get center
	double cameraAngle = camera.angle - TWO_PI / 4.0; 
	for (int i = 0; i < sprites.size(); i++)
	{
		Sprite* sprite = &sprites[i];
		//translate
		double dx = sprite->x - camera.x;
		double dy = sprite->y - camera.y;
		printf("dx:%f dy:%f\n",dx,dy);
		//rotate
		dx = dx * cos(cameraAngle) + dy * sin(cameraAngle);
		dy = -dx* sin(cameraAngle) + dy * cos(cameraAngle);
		
		sprite->dx = dx;
		sprite->dy = dy;
		//sqrt can be avoid
		sprite->distance = sqrt(dx*dx + dy*dy);
	}
	std::sort(sprites.begin(), sprites.end(), sortSprites);


	//printf("cameraAngle:%f", cameraAngle);
	for (int i = 0; i < sprites.size(); i++)
	{
		Sprite* sprite = &sprites[i];
		printf("x:%f y:%f\n",sprite->dx,sprite->dy);
	//	printf("fov:%f\n",camera.fov);
		if(sprite->dx<-camera.fov/2 or sprite->dx>camera.fov/2 or sprite->dy<=0){
			continue;
		}
		//	printf("distance:%f\n",sprite->dy);
		int lineHeight, drawStart,drawEnd;
		countVertY(perpDist,&lineHeight,&drawStart,&drawEnd);
		sprite->dx += camera.fov/2.0;
		int startX = sprite->dx/camera.fov*plane.height;
		int endX = startX + lineHeight;
		
		Texture* texture = &spritesTextures[sprite->textureId];
		Color** pixels = texture->pixels;
		double pixelYadd = (texture->height-1)/(double) lineHeight;
		for(int x = startX; x<=endX; x++){
			drawVertLine(x, drawStart, drawEnd, pixelYadd, texture->pixels, 0);
		}
	}
}
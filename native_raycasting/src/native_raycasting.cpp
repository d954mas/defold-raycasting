#define PI       3.14159265358979323846
#define TWO_PI       3.14159265358979323846 * 2
#define HALF_PI      PI / 2.0
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
static double viewDist = 0;
std::vector<Sprite> sprites;

static void updateViewDist(){
	viewDist = (plane.width / 2.0) / tan(camera.fov / 2.0);
	printf("viewDist:%f\n",viewDist);
}

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
	updateViewDist();
}

void setBuffer(int width, int height, dmScript::LuaHBuffer* luaBuffer){
	initBuffer(&buffer, width, height, luaBuffer);
}

void clearBuffer(){
	clearBuffer(&buffer);
}

void updatePlane(int x, int y, int endX, int endY){
	updatePlane(&plane, x, y, endX, endY);
	updateViewDist();
	free(PRE_CALC_HEIGHT_DISTANCE);
	int size = plane.height>>1;
	double scale = 0.6;
	PRE_CALC_HEIGHT_DISTANCE = (double *)malloc(sizeof(double) * size);
	for(int i = 0;  i < size; i++){
		PRE_CALC_HEIGHT_DISTANCE[i] =  size / (double)(size - i) * scale;
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
	return a.dy > b.dy;
}

static inline void countVertY(double dist, int* height, int* drawStart, int* drawEnd, double* pixelY, double* pixelYAdd){
	int lineHeight = ((int)round(viewDist/dist)) & 0xFFFFFFFE; //must be even
	*pixelYAdd = 1.0/(lineHeight-1);
	if(lineHeight > plane.height){
		int halfLineHeight = lineHeight>>1;
		*pixelY = *pixelYAdd *  ((lineHeight - plane.height)>>1);
		lineHeight = plane.height;
	}else{
		*pixelY = 0;
	}
	int halfLineHeight = lineHeight>>1;
	*drawStart = plane.halfHeight - halfLineHeight;
	*drawEnd =  plane.halfHeight + halfLineHeight - 1;
	*height = lineHeight;
}

static inline void drawVertLine(int x, int drawStart, int drawEnd,double pixelY, double pixelYAdd, Color** pixels, int pixelX){
	//printf("drawStart:%d drawEnd:%d\n", drawStart, drawEnd);
	for (int y = drawStart; y < drawEnd; y++) {
		setPixel(&buffer, x, y, &pixels[(int)pixelY][pixelX]);
		pixelY += pixelYAdd;
	}
}

static inline void drawWall(int x, int drawStart, int drawEnd, double pixelY, double pixelYAdd, double textureX, int textureId){
	Texture* texture = &textures[textureId];
	int pixelX = (int)( (texture->width-1)* textureX);
	pixelY = pixelY * (texture->height-1);
	pixelYAdd = pixelYAdd * (texture->height-1);
	drawVertLine(x, drawStart, drawEnd, pixelY, pixelYAdd, texture->pixels, pixelX);
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
		//draw wall
		int lineHeight, drawStart,drawEnd;
		double pixelY, pixelYAdd;
		countVertY(perpDist,&lineHeight,&drawStart,&drawEnd,&pixelY, &pixelYAdd);
		int textureId = map.walls[mapY][mapX];
		//printf("perpDist:%f lineHeight:%f\n", perpDist, lineHeight); 
		drawWall(x,drawStart, drawEnd, pixelY, pixelYAdd, textureX, textureId);
		//printf("DrawStart:%d drawEnd:%d\n", drawStart, drawEnd); 
		//draw floor and ceilings
		int** floors =  map.floors;
		int** ceils =  map.ceils;
		for(int y = 0; y < drawStart; y++){
		//	printf("draw\n");
			double currentDist = PRE_CALC_HEIGHT_DISTANCE[y];
			double weight = currentDist/perpDist;
			double weight2 = 1.0 - weight;
			//printf("weight:%f\n", weight);
			double floorX = (weight * endPositionX + weight2 * camera.x);
			double floorY = (weight * endPositionY + weight2 * camera.y);
		//	printf("floorX:%f floorY:%f\n", floorX, floorY);
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);
			int floorId = floors[cellY][cellX];
			int ceilId = ceils[cellY][cellX];
			//printf("draw2\n");
			//printf("floor id:%d\n", floorId);
			Texture* floorTexture = &textures[floorId];
			int textureX = (floorX - (int)floorX) * floorTexture->width;
			int textureY = (floorY - (int)floorY) * floorTexture->height;
		//	printf("draw3\n");
	//		printf("textureX:%d textureY:%d\n", textureX, textureY);
			Color *color = &(floorTexture->pixels[textureY][textureX]);
		//	printf("draw4\n");
		//	printf("x:%d y:%d\n", x, y);
			setPixel(&buffer, x, y, color);
		//	printf("draw5\n");

			floorTexture = &textures[ceilId];
			//floor and ceil use same width
		//	textureX = (int)(modf(floorX,&n) * floorTexture->width);
		//	textureY = (int)(modf(floorY,&n) * floorTexture->height);
			color = &(floorTexture->pixels[textureY][textureX]);
		//printf("y:%d\n",plane.height-y-1);
			setPixel(&buffer, x, plane.height - y-10, color);
		}
	}
	
	//draw sprites
	//add 0.5 to get center
	double cameraAngle = TWO_PI - (camera.angle - HALF_PI); 
	//printf("cameraAngle:%f\n",cameraAngle);
	double cameraCos = cos(cameraAngle);
	double cameraSin = sin(cameraAngle);
	for (int i = 0; i < sprites.size(); i++)
	{
		Sprite* sprite = &sprites[i];
		//translate
		double dx = sprite->x - camera.x;
		double dy = sprite->y - camera.y;
		//printf("dx:%f dy:%f\n",dx,dy);
		//rotate
		double rotatedDx =- dx * cameraSin + dy * cameraCos;
		double rotatedDy = dx * cameraCos + dy * cameraSin; 
		sprite->dx = rotatedDx;
		sprite->dy = rotatedDy;
	}
	std::sort(sprites.begin(), sprites.end(), sortSprites);

	int halfPlaneWidth =  plane.width>>1;
	//printf("cameraAngle:%f", cameraAngle);
	for (int i = 0; i < sprites.size(); i++)
	{
		Sprite* sprite = &sprites[i];
		//printf("rotated x:%f y:%f\n",sprite->dx,sprite->dy);
		if(sprite->dy<=0){
			continue;
		}
		
		int lineHeight, drawStart,drawEnd;
		double pixelY, pixelYAdd;
		countVertY(sprite->dy,&lineHeight,&drawStart,&drawEnd,&pixelY, &pixelYAdd);
		int halfLineHeight = lineHeight>>1;
		//sprite->dx += camera.fov/2.0;
		int centerX = halfPlaneWidth - (sprite->dx*lineHeight);
		int startX = centerX - halfLineHeight;
		int endX = centerX + halfLineHeight;
	   // printf("centerX:%d startX:%d endX:%d\n ",centerX, startX, endX);
		Texture* texture = &spritesTextures[sprite->textureId];
		Color** pixels = texture->pixels;
		double pixelX=0;
		double addX = (double)(texture->width-1)/(endX-startX);
		if(startX<0){
			pixelX = pixelYAdd * - startX;
		}	
		if(startX < 0){
			startX = 0;
		}
		if(endX > plane.endX-1){
			endX = plane.endX-1;
		}
		for(int x = startX; x<=endX; x++){
			if(sprite->dy <=distanceBuffer[x]){
				drawVertLine(x, drawStart, drawEnd, pixelY, pixelYAdd, texture->pixels, (int)pixelX);
				pixelX+=addX;
			}
		}
	}
}
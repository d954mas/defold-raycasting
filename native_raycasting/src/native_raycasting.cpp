#define PI       3.14159265358979323846
#define TWO_PI       3.14159265358979323846 * 2
#define HALF_PI      PI / 2.0
#define ROUNDNUM(x) (int)(x + 0.5)
#include <dmsdk/sdk.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include "world_structures.h"
#include "buffer.h"
#include "raycasting.h"
#include "texture.h"
#include "png.h"

static Camera CAMERA;
static Buffer BUFFER;
static Map MAP;
static Plane PLANE;
static std::vector<Texture> TEXTURES;
static std::vector<double> PRE_CALC_HEIGHT_DISTANCE;
static std::vector<double> DISTANCE_BUFFER;
static std::vector<Sprite> SPRITES;
static std::vector<double> ANGLES;
static double viewDist = 0;

static void viewDistanceUpdated(){
	viewDist = PLANE.width/(2 * tan(CAMERA.fov/2.0));
	DISTANCE_BUFFER.resize(PLANE.width);
	PRE_CALC_HEIGHT_DISTANCE.resize(PLANE.width);
	PRE_CALC_HEIGHT_DISTANCE.clear();
	ANGLES.resize(PLANE.halfWidth);
	ANGLES.clear();
	for(int x=1;x<=PLANE.halfWidth;x++){
		ANGLES.push_back(atan(x/viewDist));
	}
	double scale = viewDist/PLANE.height;
	for(int i = 0;  i < PLANE.halfHeight; i++){
		PRE_CALC_HEIGHT_DISTANCE.push_back(PLANE.halfHeight / (double)(PLANE.halfHeight - i) * scale);
	}
}

void setCameraFov(double fov){
	setCameraFov(&CAMERA, fov);
	viewDistanceUpdated();
}

void updatePlane(int x, int y, int endX, int endY){
	updatePlane(&PLANE, x, y, endX, endY);
	viewDistanceUpdated();
}

void addSprite(double x, double y, int textureId){
	Sprite sprite = {x,y,textureId,0};
	SPRITES.push_back(sprite); 
}

void loadTexture(dmScript::LuaHBuffer* luaBuffer, int id){
	if(id>=TEXTURES.size()){
		TEXTURES.resize(id+1);
	}
	decodeToTexture(luaBuffer->m_Buffer, &TEXTURES[id]);
}

void updateCamera(double x, double y, double angle){
	updateCamera(&CAMERA, x, y, angle);
}

void setBuffer(int width, int height, dmScript::LuaHBuffer* luaBuffer){
	initBuffer(&BUFFER, width, height, luaBuffer);
}

void clearBuffer(){
	clearBuffer(&BUFFER);
}

void setMap(lua_State* L){
	parseMap(L, &MAP);
}

void castSingleRay(double rayAngle, double *perpDist, double *endX, 
	double *endY, int *mapXResult, int *mapYResult, double *textureX){
		castRay(&CAMERA, MAP.walls, rayAngle, perpDist, endX, 
			endY, mapXResult, mapYResult, textureX);
}

static bool sortSprites(const Sprite &a, const Sprite &b){
	return a.dy > b.dy;
}

static inline void countVertY(double dist, int* height, int* drawStart, int* drawEnd, double* pixelY, double* pixelYAdd){
	int lineHeight = ((int)(viewDist/dist + 0.5)) & 0xFFFFFFFE; //must be even
	//int lineHeight = ((int)round(viewDist/dist)) & 0xFFFFFFFE; //must be even
	*pixelYAdd = 1.0/(lineHeight-1);
	*height = lineHeight;
	if(lineHeight > PLANE.height){
		int halfLineHeight = lineHeight>>1;
		*pixelY = *pixelYAdd *  ((lineHeight - PLANE.height)>>1);
		lineHeight = PLANE.height;
	}else{
		*pixelY = 0;
	}
	int halfLineHeight = lineHeight>>1;
	*drawStart = PLANE.halfHeight - halfLineHeight;
	*drawEnd =  PLANE.halfHeight + halfLineHeight - 1;
}

static inline void drawVertLine(int x, int drawStart, int drawEnd,double pixelY, double pixelYAdd, Color** pixels, int pixelX){
	//printf("drawStart:%d drawEnd:%d\n", drawStart, drawEnd);
	Color* line = pixels[pixelX];
	for (int y = drawStart; y < drawEnd; y++) {
		//setPixel(&buffer, x, y, &pixels[(int)pixelY][pixelX]);
		setPixel(&BUFFER, x, y, &line[(int)pixelY]);
		//bufferPixels[y][0] = line[(int)pixelY].colors[0];
		//memcpy(&bufferPixels[y][0], &line[(int)pixelY].colors[0], 3);
		//memcpy(&buffer.stream[(y * buffer.width + x) * 3], &line[(int)pixelY].colors[0], 3);
		pixelY += pixelYAdd;
		//printf("pixelY:%f\n", pixelY);
	}
}

static inline void drawVertLineTransparent(int x, int drawStart, int drawEnd,double pixelY, double pixelYAdd, Color** pixels, int pixelX){
	Color* line = pixels[pixelX];
	for (int y = drawStart; y < drawEnd; y++) {
		setPixelTransparent(&BUFFER, x, y, &line[(int)pixelY]);
		pixelY += pixelYAdd;
	}
}

static inline void drawWall(int x, int drawStart, int drawEnd, double pixelY, double pixelYAdd, double textureX, int textureId){
	Texture* texture = &TEXTURES[textureId];
	int pixelX = (int)( (texture->widthM)* textureX);
	pixelY = pixelY * (texture->heightM);
	pixelYAdd = pixelYAdd * (texture->heightM);
	drawVertLine(x, drawStart, drawEnd, pixelY, pixelYAdd, texture->pixels, pixelX);
}

static inline void drawSprites(){
	//draw sprites
	double cameraAngle = TWO_PI - (CAMERA.angle - HALF_PI); 
	//printf("cameraAngle:%f\n",cameraAngle);
	double cameraCos = cos(cameraAngle);
	double cameraSin = sin(cameraAngle);
	for (int i = 0; i < SPRITES.size(); i++)
	{
		Sprite* sprite = &SPRITES[i];
		//translate
		double dx = sprite->x - CAMERA.x;
		double dy = sprite->y - CAMERA.y;
		//printf("dx:%f dy:%f\n",dx,dy);
		//rotate
		double rotatedDx =- dx * cameraSin + dy * cameraCos;
		double rotatedDy = dx * cameraCos + dy * cameraSin; 
		sprite->dx = rotatedDx;
		sprite->dy = rotatedDy;
	}
	std::sort(SPRITES.begin(), SPRITES.end(), sortSprites);

	//printf("cameraAngle:%f", cameraAngle);
	for (int i = 0; i < SPRITES.size(); i++)
	{
		Sprite* sprite = &SPRITES[i];
		//printf("rotated x:%f y:%f\n",sprite->dx,sprite->dy);
		if(sprite->dy<=0){
			continue;
		}
		double lineHeightDouble = viewDist/sprite->dy;
		int lineHeight, drawStart,drawEnd;
		double pixelY, pixelYAdd;
		countVertY(sprite->dy,&lineHeight,&drawStart,&drawEnd,&pixelY, &pixelYAdd);
		Texture* texture = &TEXTURES[sprite->textureId];
		pixelY = pixelY * (texture->heightM);
		pixelYAdd = pixelYAdd * (texture->heightM);
		int halfLineHeight = lineHeight>>1;
		//sprite->dx += camera.fov/2.0;
		int centerX = PLANE.halfWidth - (sprite->dx*lineHeightDouble);
		int startX = centerX - halfLineHeight;
		int endX = centerX + halfLineHeight;
		// printf("centerX:%d startX:%d endX:%d\n ",centerX, startX, endX);

		Color** pixels = texture->pixels;
		double pixelX=0;
		double addX = (double)(texture->widthM)/(endX-startX);
		if(startX<0){
			pixelX = addX * -startX;
			startX = 0;
		}	
		if(endX > PLANE.endX-1){
			endX = PLANE.endX-1;
		}
		//printf("pixelY%f addY:%f\n",pixelY,pixelYAdd);
		for(int x = startX; x<endX; x++){
			if(sprite->dy <=DISTANCE_BUFFER[x]){
				drawVertLineTransparent(x, drawStart, drawEnd, pixelY, pixelYAdd, texture->pixels, (int)pixelX);
				//printf("pixelX:%f\n",pixelX);
			}
			pixelX+=addX;
		}
	}
}

static void inline drawFloorsAndCeilings(int x, int drawStart, double perpDist, double endX, double endY){
	//draw floor and ceilings
	for(int y = 0; y < drawStart; y++){
		double currentDist = PRE_CALC_HEIGHT_DISTANCE[y];
		double weight = currentDist/perpDist;
		double weight2 = 1.0 - weight;
		//printf("weight:%f\n", weight);
		double floorX = (weight * endX + weight2 * CAMERA.x);
		double floorY = (weight * endY + weight2 * CAMERA.y);
		//	printf("floorX:%f floorY:%f\n", floorX, floorY);
		int cellX = (int)(floorX);
		int cellY = (int)(floorY);
		int floorId = MAP.floors[cellY][cellX];
		int ceilId = MAP.ceils[cellY][cellX];
		//printf("floor id:%d\n", floorId);
		Texture* floorTexture = &TEXTURES[floorId];
		int textureX = (floorX - (int)floorX) * floorTexture->widthM;
		int textureY = (floorY - (int)floorY) * floorTexture->heightM;
		//		printf("textureX:%d textureY:%d\n", textureX, textureY);
		Color *color = &(floorTexture->pixels[textureX][textureY]);
		//	printf("draw4\n");
		//	printf("x:%d y:%d\n", x, y);
		setPixel(&BUFFER, x, y, color);
		//	printf("draw5\n");
		floorTexture = &TEXTURES[ceilId];
		color = &(floorTexture->pixels[textureX][textureY]);
		setPixel(&BUFFER, x, PLANE.height - y-1, color);
	}
}

void castRays(){
	double perpDist, endX, endY, textureX;
	int mapX, mapY;
	int** floors =  MAP.floors;
	int** ceils =  MAP.ceils;
	int lineHeight, drawStart,drawEnd;
	double pixelY, pixelYAdd;
	for(int i=0; i<PLANE.halfWidth; i++){
		double rayAngle = ANGLES[i];
		for(int j=0;j<2;j++){
			double currentAngle = (j==1) ? -rayAngle : rayAngle;
			int x = (j==1) ? (PLANE.halfWidth) - i-1 : (PLANE.halfWidth) + i;
			//cast ray
			castSingleRay(currentAngle, &perpDist, &endX, &endY, &mapX, &mapY, &textureX);
			DISTANCE_BUFFER[x] =  perpDist;
			//draw wall
			countVertY(perpDist,&lineHeight,&drawStart,&drawEnd,&pixelY, &pixelYAdd);
			drawWall(x,drawStart, drawEnd, pixelY, pixelYAdd, textureX, MAP.walls[mapY][mapX]);
			drawFloorsAndCeilings(x, drawStart, perpDist, endX, endY);
		}
	}
	drawSprites();
}
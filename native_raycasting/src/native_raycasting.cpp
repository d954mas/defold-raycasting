// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME NativeRaycasting
#define LIB_NAME "NativeRaycasting"
#define MODULE_NAME "native_raycasting"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include <math.h>
#include <stdlib.h>

struct Camera {
	int x;
	int y;
	int width;
	int height;
} camera;
struct Texture {
	int width;
	int height;
	struct Color **pixels;
};

struct Color {
	int r;
	int g;
	int b;
};

struct Map {
	int width;
	int height;
	int **walls;
	int **floors;
	int **ceils;
} map;

static int BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_SIZE;
static uint8_t* STREAM = 0x0;
static struct Texture *wallTextures;

static int setPixel(int x, int y, Color *color){
	int id = (y * BUFFER_WIDTH + x) * 3;
	//if(id > 480*640 *3 || id <0){
		//printf("bad id:%d", id);
		//return 1;
		//}
		//printf("id:%d", id);
		//	int r = color >> 16;
		//int g = (color & 0x00FF00) >> 8;
		//int b= color & 0x0000ff;
		STREAM[id] = color->r;
		STREAM[id + 1] = color->g;
		STREAM[id + 2] = color->b;
		return 1;
	}

static int castRay(double startX, double startY, double cameraAngle, double rayAngle
	,double *perpDist, double *catetX,  double *catetY, int *mapXResult, int *mapYResult, double *textureX){
		double angle = cameraAngle + rayAngle;
		int mapX = (int)startX, mapY = (int)startY;
		double angleSin = sin(angle), angleCos = cos(angle);
		double dx = 1.0/angleSin, dy = 1.0/angleCos;
		double absDx = fabs(dx), absDy = fabs(dy);
		double sx, sy;
		int stepX, stepY;
		if (dx>0){
			sx = (mapX - startX + 1) * absDx;
			stepX = 1;
		}else{
			sx = (startX  - mapX) * absDx;
			stepX = - 1;
		}

		if (dy>0){
			sy = (mapY - startY + 1) * absDy;
			stepY = 1;
		}else{
			sy = (startY  - mapY) * absDy;
			stepY = - 1;
		}
		bool hitX = true;
		while (true)
		{
			if(sx < sy){
				mapX = mapX + stepX;
				sx = sx + absDx;
				hitX = true;
			}else{
				mapY = mapY + stepY;
				sy = sy + absDy;
				hitX = false;
			}

			if(map.walls[mapY][mapX] > 0){
				double dist; 
				if(hitX){
					sx = sx - absDx; //remove last dx
					dist = sx;
				}else{
					sy = sy - absDy; //remove last dy
					dist = sy;
				}
				*catetX = dist * angleSin;
				*catetY = dist * angleCos;
				double n;
				if(hitX){
					*textureX = modf(startY + *catetY,&n);
				}else{
					*textureX = modf(startX + *catetX,&n);
				}
				*perpDist = dist *  cos(rayAngle);
				*mapXResult = mapX;
				*mapYResult = mapY;
				return 0;
			}
		}	
	} 
	
static int castRays(double startX, double startY, double cameraAngle, double fov){
	double rayAngle = - fov / 2.0;
	double cameraRayAngle = fov/camera.width;
	double perpDist; 
	double catetX;
	double catetY;
	double textureX;
	int mapX;
	int mapY;
	int halfCameraHeight = camera.height/2;
	for (int i=0; i < camera.width; i++){
		castRay(startX, startY, cameraAngle, rayAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
		rayAngle += cameraRayAngle;
		//fix fov and aspect here
		int halfLineHeight = (int)(round(camera.height / perpDist)/2.0 + 0.5);
		int drawStart = halfCameraHeight - halfLineHeight;
		int drawEnd =  halfLineHeight + halfCameraHeight;
		if(drawStart < 0){
			drawStart = 0;
		} 
		if(drawEnd > camera.height){
			drawEnd = camera.height;
		}
		//draw vert line
		int textureId = map.walls[mapY][mapX]-1;
		Texture *wall = &wallTextures[textureId];
		int pixelX = (int)((wall->width-1) * textureX);
		double yWidth = drawEnd - drawStart;
		for (int y = drawStart; y <= drawEnd; y++) {
			int pixelY = (int)((y - drawStart) / yWidth * 63);
			Color *color = &(wall->pixels[pixelY][pixelX]);
			setPixel(i, y, color);
		}
	}
	return 0;
}

static int castRaysLua(lua_State* L)
{	
	double startX = lua_tonumber(L, 1), startY = lua_tonumber(L, 2);
	double cameraAngle = lua_tonumber(L, 3), rayAngle = lua_tonumber(L, 4);
	castRays(startX, startY, cameraAngle, rayAngle);
	return 0;
}

static int castRayLua(lua_State* L)
{	
	double startX = lua_tonumber(L, 1), startY = lua_tonumber(L, 2);
	double cameraAngle = lua_tonumber(L, 3), rayAngle = lua_tonumber(L, 4);
	double perpDist; 
	double catetX;
	double catetY;
	double textureX;
	int mapX;
	int mapY;
	castRay(startX, startY, cameraAngle, rayAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
	lua_pushnumber(L, perpDist);
	lua_pushnumber(L, catetX);
	lua_pushnumber(L, catetY);
	lua_pushnumber(L, mapX);
	lua_pushnumber(L, mapY);
	lua_pushnumber(L, textureX);
	return 6;
}



static int setPixelLua(lua_State* L){
	int x = (int) lua_tonumber(L, 1);
	int y = (int) lua_tonumber(L, 2);
	int color = (int) lua_tonumber(L, 3);
	//setPixel(x, y, color);
	return 0;
}

static int vertLine(lua_State* L){
	int x = (int) lua_tonumber(L, 1);
	int startY = (int) lua_tonumber(L, 2);
	int endY = (int) lua_tonumber(L, 3);
	int textureId = (int) lua_tonumber(L, 4);
	Texture *wall = &wallTextures[textureId];
	double textureX = lua_tonumber(L, 5);
	int pixelX = (int)((wall->width-1) * textureX);
	double yWidth = endY - startY;
	for (int y = startY; y <= endY; y++) {
		int pixelY = (int)((y - startY) / yWidth * 63);
		Color *color = &(wall->pixels[pixelY][pixelX]);
		setPixel(x, y, color);
	}
	return 1;
}

static int floorCasting(lua_State* L){
	int startX = (int) lua_tonumber(L, 1);
	int startY = (int) lua_tonumber(L, 2);
	int endY = (int) lua_tonumber(L, 3);

	double cameraX = lua_tonumber(L, 4);
	double cameraY = lua_tonumber(L, 5);
	double cameraHeight = lua_tonumber(L, 6);

	double endPositionX = lua_tonumber(L, 7);
	double endPositionY = lua_tonumber(L, 8);

	double perpDist = lua_tonumber(L, 9);

	int halfHeight = cameraHeight/2;
	bool isFloor = startY >=endY;
	int loopStartY = isFloor ? startY : endY;
	int loopEndY = isFloor ? endY : startY;
	for(int y = loopStartY; y > loopEndY; y--){
		double currentDist;
		int **floors;
		if(isFloor){
			//currentDist =  PRE_CALC_HEIGHT_DISTANCE[halfHeight - y];
			floors = map.floors; 
		}else{
			//currentDist =   PRE_CALC_HEIGHT_DISTANCE[y - halfHeight];
			floors = map.ceils; 
		}	
		currentDist =   1;
		double weight = currentDist / perpDist;
		double floorX = (weight * endPositionX + (1.0 - weight) * cameraX);
		double floorY = (weight * endPositionY + (1.0 - weight) * cameraY);
		int cellX = (int)(floorX);
		int cellY = (int)(floorY);
		int floorId = floors[cellY][cellX];
		double n;
		int textureX = round(modf(floorX,&n) * 63);
		int textureY = round(modf(floorY,&n) * 63);
		Texture *floorTexture = &wallTextures[floorId-1];
		Color *color = &(floorTexture->pixels[textureY][textureX]);
		setPixel(startX, y, color);
	}	
	return 0;
}

static int ** createTwoArrayInt(int width, int height){
	int **array = (int**)malloc(height * sizeof(int*));
	for (int i = 0; i<height; i++) 
	{
		array[i] = (int*)malloc(sizeof(int) * width);
	}
	return array;
}

static Color ** createTwoArrayColor(int width, int height){
	Color **array = (Color**)malloc(height * sizeof(Color));
	for (int i = 0; i<height; i++) 
	{
		array[i] = (Color*)malloc(sizeof(Color) * width);
	}
	return array;
}

static int clearArrayInt(int **array, int height){
	for (int i = 0; i < height; i++)
	{
		free(array[i]);
	}
	free(array);
	return 0;
}

static int clearArrayColor(Color **array, int height){
	for (int i = 0; i < height; i++)
	{
		free(array[i]);
	}
	free(array);
	return 0;
}

static int initCamera(lua_State* L){
	camera.x = (int) lua_tonumber(L, 1);
	camera.y = (int) lua_tonumber(L, 2);
	camera.width = (int) lua_tonumber(L, 3);
	camera.height = (int) lua_tonumber(L, 4);
	return 0;
}

static Texture getTextureFromStack(lua_State* L){
	struct Texture texture;
	lua_getfield(L, -1, "width");
	lua_getfield(L, -2, "height");
	texture.width = (int)lua_tonumber(L, -2);
	texture.height = (int)lua_tonumber(L, -1);
	lua_pop(L, 2);
	//printf("Texture width:%d height:%d\n", texture.width, texture.height);
	texture.pixels = createTwoArrayColor(texture.width, texture.height);
	for(int y = 0; y < texture.height; y++){
		for(int x = 0; x < texture.width; x++){
			int id = y * texture.width + x + 1;
			lua_rawgeti(L,-1,id);
			int color = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
			struct Color colorStruct;
			colorStruct.r = color >> 16;
			colorStruct.g = (color & 0x00FF00) >> 8;
			colorStruct.b= color & 0x0000ff;
			texture.pixels[y][x] = colorStruct;
		}
	}
	return texture;
}

static int initWallTextures(lua_State* L){
	int n = luaL_getn(L, 1);  // get size of table
	free(wallTextures);
	wallTextures = (Texture *)malloc(sizeof(Texture) * n);
	for(int i = 0;  i < n; i++){
		lua_rawgeti(L, -1, i+1);
		wallTextures[i] = getTextureFromStack(L);
		lua_pop(L, 1);
	}
	return 0;
}

static int ** parseMap(lua_State* L, int width, int height){
	int **map = createTwoArrayInt(width, height);
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			lua_rawgeti(L,-1,y + 1);
			lua_rawgeti(L,-1,x + 1);
			int cell = (int)lua_tonumber(L, -1);
			lua_pop(L, 2);
			map[y][x] = cell;
		}
	}
	return map;
}

static int setMap(lua_State* L){
	lua_getfield(L, 1, "WIDTH");
	lua_getfield(L, 1, "HEIGHT");
	int width = lua_tonumber(L, -2);
	int height = lua_tonumber(L, -1);
	lua_pop(L, 2);
	
	clearArrayInt(map.walls, map.height);
	clearArrayInt(map.floors, map.height);
	clearArrayInt(map.ceils, map.height);
	map.width = width;
	map.height = height;
	lua_getfield(L, 1, "MAP");
	map.walls = parseMap(L, width, height);
	lua_pop(L, 1);
	lua_getfield(L, 1, "FLOORS");
	map.floors = parseMap(L, width, height);
	lua_pop(L, 1);
	lua_getfield(L, 1, "CEILS");
	map.ceils = parseMap(L, width, height);
	lua_pop(L, 1);
	printf("here3\n");
	return 0;
}

static int initBuffer(lua_State* L){
	BUFFER_WIDTH = (int) lua_tonumber(L, 1);
	BUFFER_HEIGHT = (int) lua_tonumber(L, 2);
	BUFFER_SIZE = BUFFER_WIDTH * BUFFER_HEIGHT;
	dmScript::LuaHBuffer* buffer = dmScript::CheckBuffer(L, 3);
	uint32_t size_ignored = 0;
	uint32_t components = 0;
	uint32_t stride = 0;
	dmBuffer::HBuffer hBuffer = buffer->m_Buffer;
	dmBuffer::Result r = dmBuffer::GetStream(hBuffer, dmHashString64("rgb"), (void**)&STREAM, &size_ignored, &components, &stride);
	return 1;
}

static int clearBuffer(lua_State* L){
	for(int i=0; i< BUFFER_SIZE * 3-1;i+=3){
		STREAM[i] = 0;
		STREAM[i+1] = 0;
		STREAM[i+2] = 0;
	}
	return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{	
	{"init_buffer", initBuffer},
	{"clear_buffer", clearBuffer},
	{"init_camera", initCamera},
	{"init_wall_textures",initWallTextures},
	{"set_map", setMap},
	{"cast_ray", castRayLua},
	{"cast_rays", castRaysLua},
	{"vert_line", vertLine},
	{"floor_casting", floorCasting},
	{"set_pixel", setPixelLua},
	{0, 0}
};

static void LuaInit(lua_State* L)
{
	int top = lua_gettop(L);
	// Register lua names
	luaL_register(L, MODULE_NAME, Module_methods);
	lua_pop(L, 1);
	assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
	// Init Lua
	LuaInit(params->m_L);
	printf("Registered %s Extension\n", MODULE_NAME);
	return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, 0, 0, FinalizeMyExtension)
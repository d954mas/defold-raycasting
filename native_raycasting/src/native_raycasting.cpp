// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME NativeRaycasting
#define LIB_NAME "NativeRaycasting"
#define MODULE_NAME "native_raycasting"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include <math.h>

static int BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_SIZE;
static uint8_t* STREAM = 0x0;

//start_x, start_y, angle, ray_angle, cells
//заранее зенести cells в массив.Чтобы не дергать луа
double *PRE_CALC_HEIGHT_DISTANCE;
static int castRay(lua_State* L)
{	
	double startX = lua_tonumber(L, 1), startY = lua_tonumber(L, 2);
	double cameraAngle = lua_tonumber(L, 3), rayAngle = lua_tonumber(L, 4);
	double angle = cameraAngle + rayAngle;
	int mapX = (int)ceil(startX), mapY = (int)ceil(startY);
	double angleSin = sin(angle), angleCos = cos(angle);
	double dx = 1.0/angleSin, dy = 1.0/angleCos;
	double absDx = fabs(dx), absDy = fabs(dy);
	double sx, sy;
	int stepX, stepY;
	if (dx>0){
		sx = (mapX - startX) * absDx;
		stepX = 1;
	}else{
		sx = (1 + startX  - mapX) * absDx;
		stepX = - 1;
	}
	
	if (dy>0){
		sy = (mapY - startY) * absDy;
		stepY = 1;
	}else{
		sy = (1 + startY  - mapY) * absDy;
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

		lua_rawgeti(L,-1,mapY);
		lua_rawgeti(L,-1,mapX);
		int cell = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pop(L, 1);
		if(cell > 0){
			double dist; 
			if(hitX){
				sx = sx - absDx; //remove last dx
				dist = sx;
			}else{
				sy = sy - absDy; //remove last dy
				dist = sy;
			}
			double catetX = dist * angleSin;
			double catetY = dist * angleCos;
			double n, textureX;
			if(hitX){
				textureX = modf(startY + catetY,&n);
			}else{
				textureX = modf(startX + catetX,&n);
			}
			double perpDist = dist *  cos(rayAngle);

			lua_pushnumber(L, perpDist);
			lua_pushnumber(L, catetX);
			lua_pushnumber(L, catetY);
			lua_pushnumber(L, mapX);
			lua_pushnumber(L, mapY);
			lua_pushnumber(L, textureX);
			return 6;
		}
	}
}

static int setPixel(int x, int y, int color){
	int id = (y * BUFFER_WIDTH + x) * 3;
	if(id > 480*640 *3 || id <0){
		printf("bad id:%d", id);
		return 1;
	}
	//printf("id:%d", id);
	int r = color >> 16;
	int g = (color & 0x00FF00) >> 8;
	int b= color & 0x0000ff;
	STREAM[id] = r;
	STREAM[id + 1] = g;
	STREAM[id + 2] = b;
	return 1;
}

static int setPixelLua(lua_State* L){
	int x = (int) lua_tonumber(L, 1);
	int y = (int) lua_tonumber(L, 2);
	int color = (int) lua_tonumber(L, 3);
	setPixel(x, y, color);
	return 0;
}

static int vertLine(lua_State* L){
	int x = (int) lua_tonumber(L, 1);
	int startY = (int) lua_tonumber(L, 2);
	int endY = (int) lua_tonumber(L, 3);
	
	lua_pushstring(L, "width");
	lua_gettable(L, 4);  // get table[key]
	int wallWidth = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);  // remove number from stack

	double textureX = lua_tonumber(L, 5);
	
	int pixelX = (int)((wallWidth-1) * textureX) + 1;
	double yWidth = endY - startY;
	for (int y = startY; y <= endY; y++) {
		int pixelY = (int)((y - startY) / yWidth * 63) +1;
		int id = (pixelY - 1) * wallWidth + pixelX;
		lua_pushnumber(L, id);
		lua_gettable(L, 4);  // get table[key]
		int color = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);  // remove number from stack
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
		if(isFloor){
			currentDist =  PRE_CALC_HEIGHT_DISTANCE[halfHeight - y];
		}else{
			currentDist =   PRE_CALC_HEIGHT_DISTANCE[y - halfHeight];
		}	
		double weight = currentDist / perpDist;
		double floorX = (weight * endPositionX + (1.0 - weight) * cameraX);
		double floorY = (weight * endPositionY + (1.0 - weight) * cameraY);
		int cellX = ceil(floorX);
		int cellY = ceil(floorY);
		lua_rawgeti(L,-1,cellY);
		lua_rawgeti(L,-1,cellX);
		int floorId = (int)lua_tonumber(L, -1);
		lua_pop(L, 2);
		double n;
		int textureX = round(modf(floorX,&n) * 63) + 1;
		int textureY = round(modf(floorY,&n) * 63) + 1;
		int id = (textureY - 1) * 64 + textureX;
		lua_rawgeti(L,-2, floorId);
		lua_rawgeti(L,-1, id);
		int color = (int)lua_tonumber(L, -1);
		lua_pop(L, 2);
		
		setPixel(startX, y, color);
	}	
	return 0;
}

static int ** createTwoArrayInt(int height, int width){
	int **array = (int**)malloc(height * sizeof(int*));
	for (int i = 0; i<height; i++) 
	{
		array[i] = (int*)malloc(sizeof(int) * width);
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

static int initCamera(lua_State* L){
	int width = (int) lua_tonumber(L, 1);
	int height = (int) lua_tonumber(L, 2);
	free(PRE_CALC_HEIGHT_DISTANCE);
	double halfHeight = height/2.0;
	int size = ceil(halfHeight);
	PRE_CALC_HEIGHT_DISTANCE = (double *)malloc(sizeof(double) * size);
	for(int i = 0;  i < size; i++){
		PRE_CALC_HEIGHT_DISTANCE[i] =  halfHeight / i;
	}
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
	{"cast_ray", castRay},
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
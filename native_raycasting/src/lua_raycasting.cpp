// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME NativeRaycasting
#define LIB_NAME "NativeRaycasting"
#define MODULE_NAME "native_raycasting"
// include the Defold SDK
#include <dmsdk/sdk.h>
#include "native_raycasting.h"

static int updateCamera(lua_State* L){
	int cameraId = (int)lua_tonumber(L, 1);
	double posX = lua_tonumber(L, 2);
	double posY = lua_tonumber(L, 3);
	double angle = lua_tonumber(L, 4);
	updateCamera(cameraId, posX, posY, angle);
	return 0;
}

static int setBufferLua(lua_State* L){
	int width = (int) lua_tonumber(L, 1);
	int height = (int) lua_tonumber(L, 2);
	dmScript::LuaHBuffer* buffer = dmScript::CheckBuffer(L, 3);
	setBuffer(width, height, buffer);
	return 0;
}

static int setMapLua(lua_State* L){
	setMap(L);
	return 0;
}

static int castRayLua(lua_State* L){
	int cameraId = (int)lua_tonumber(L, 1);
	double rayAngle = lua_tonumber(L, 2);
	double perpDist; 
	double catetX;
	double catetY;
	double textureX;
	int mapX;
	int mapY;
	castSingleRay(cameraId, rayAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
	lua_pushnumber(L, perpDist);
	lua_pushnumber(L, catetX);
	lua_pushnumber(L, catetY);
	lua_pushnumber(L, mapX);
	lua_pushnumber(L, mapY);
	lua_pushnumber(L, textureX);
	return 6;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{	
	{"update_camera", updateCamera},
	{"set_buffer", setBufferLua},
	{"set_map", setMapLua},
	{"cast_ray", castRayLua},
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
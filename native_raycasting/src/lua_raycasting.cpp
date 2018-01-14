// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME NativeRaycasting
#define LIB_NAME "NativeRaycasting"
#define MODULE_NAME "native_raycasting"
// include the Defold SDK
#include <dmsdk/sdk.h>
#include "native_raycasting.h"

static int updateCameraLua(lua_State* L){
	double posX = lua_tonumber(L, 1);
	double posY = lua_tonumber(L, 2);
	double angle = lua_tonumber(L, 3);
	updateCamera(posX, posY, angle);
	return 0;
}

static int setCameraFovLua(lua_State* L){
	double fov = lua_tonumber(L, 1);
	setCameraFov(fov);
	return 0;
}

static int updatePlaneLua(lua_State* L){
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	int endX = lua_tonumber(L, 3);
	int endY = lua_tonumber(L, 4);
	updatePlane(x, y, endX, endY);
	return 0;
}

static int setBufferLua(lua_State* L){
	int width = (int) lua_tonumber(L, 1);
	int height = (int) lua_tonumber(L, 2);
	dmScript::LuaHBuffer* buffer = dmScript::CheckBuffer(L, 3);
	setBuffer(width, height, buffer);
	return 0;
}

static int clearBufferLua(lua_State* L){
	clearBuffer();
	return 0;
}

static int setMapLua(lua_State* L){
	setMap(L);
	return 0;
}

static int castRayLua(lua_State* L){
	double rayAngle = lua_tonumber(L, 1);
	double perpDist; 
	double catetX;
	double catetY;
	double textureX;
	int mapX;
	int mapY;
	castSingleRay(rayAngle, &perpDist, &catetX, &catetY, &mapX, &mapY, &textureX);
	lua_pushnumber(L, perpDist);
	lua_pushnumber(L, catetX);
	lua_pushnumber(L, catetY);
	lua_pushnumber(L, mapX);
	lua_pushnumber(L, mapY);
	lua_pushnumber(L, textureX);
	return 6;
}

static int castRaysLua(lua_State* L){
	castRays();
	return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{	
	{"update_camera", updateCameraLua},
	{"set_camera_fov", setCameraFovLua},
	{"update_plane", updatePlaneLua},
	{"set_buffer", setBufferLua},
	{"clear_buffer", clearBufferLua},
	{"set_map", setMapLua},
	{"cast_ray", castRayLua},
	{"cast_rays", castRaysLua},
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
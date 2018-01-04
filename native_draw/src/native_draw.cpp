// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME NativeDraw
#define LIB_NAME "NativeDraw"
#define MODULE_NAME "native_draw"

// include the Defold SDK
#include <dmsdk/sdk.h>

static int drawPixels(lua_State* L)
{	
	dmScript::LuaHBuffer* buffer = dmScript::CheckBuffer(L, 2);
	uint8_t* stream = 0x0;
	uint32_t size_ignored = 0;
	dmBuffer::HBuffer hBuffer = buffer->m_Buffer;
	dmBuffer::Result r = dmBuffer::GetStream(hBuffer, dmHashString64("rgb"), (void**)&stream, &size_ignored);
	//printf("size:%s",size_ignored);
	lua_getfield(L, 1, "width");
	lua_getfield(L, 1, "height");;
	int width = luaL_checknumber(L, -2);
	int height = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_pop(L, 1); 
	int size = width * height;
	lua_pushstring(L,"stream");
	lua_gettable(L, -3 );
	lua_pushstring(L,"leds");
	lua_gettable(L, -4 );
	for(int i=1; i<= size;i++){
		lua_rawgeti(L,-1,i);
		int color = luaL_checknumber(L, -1);
		lua_pop(L, 1);
		int r = color >> 16;
		int g = (color & 0x00FF00) >> 8;
		int b= color & 0x0000ff;
		int id = i * 3;
		stream[id - 3] = r;
		stream[id - 2] = g;
		stream[id - 1] = b;
		//printf("i:%s",i);
	}
	lua_pop(L,1);
	
	return 1;
}

static int drawPixelsOld(lua_State* L)
{	
	lua_getfield(L, 1, "width");
	lua_getfield(L, 1, "height");
	int width = luaL_checknumber(L, -2);
	int height = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_pop(L, 1); 
	int size = width * height;
	lua_pushstring(L,"stream");
	lua_gettable(L, -2 );
	lua_pushstring(L,"leds");
	lua_gettable(L, -3 );
	//printf("size:%s",size_ignored);
	for(int i=1; i<= size;i++){
		lua_rawgeti(L,-1,i);
		int color = luaL_checknumber(L, -1);
		//printf("i=%d color=%d \n",i,color);
		lua_pop(L, 1);
		int r = color >> 16;
		int g = (color & 0x00FF00) >> 8;
		int b= color & 0x0000ff;

		int id = i * 3;
		lua_pushnumber(L, id - 2);
		lua_pushnumber(L, r);
		lua_settable(L, -4);
		lua_pushnumber(L, id - 1);
		lua_pushnumber(L, g);
		lua_settable(L, -4);
		lua_pushnumber(L, id);
		lua_pushnumber(L, b);
		lua_settable(L, -4);
		//printf("i:%s",i);
	}
	lua_pop(L,1);

	return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
	{"draw_pixels", drawPixels},
	{"draw_pixels_old", drawPixelsOld},
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

dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
	// Init Lua
	LuaInit(params->m_L);
	printf("Registered %s Extension\n", MODULE_NAME);
	return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, 0, 0, FinalizeMyExtension)
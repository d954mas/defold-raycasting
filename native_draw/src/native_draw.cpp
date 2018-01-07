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
	double width = lua_tonumber(L, -2);
	double height = lua_tonumber(L, -1);
	lua_pop(L, 2);
	double size = width * height;
	lua_pushstring(L,"stream");
	lua_gettable(L, -3 );
	lua_pushstring(L,"leds");
	lua_gettable(L, -4 );
	for(int i=1; i<= size;i++){
		lua_rawgeti(L,-1,i);
		int color = (int)lua_tonumber(L, -1);
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

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
	{"draw_pixels", drawPixels},
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
#ifndef NATIVE_RAYCASTING
#define NATIVE_RAYCASTING
#include <dmsdk/sdk.h>

void updateCamera(int, double, double, double);
void setBuffer(int, int, dmScript::LuaHBuffer*);
void setMap(lua_State*);
void castSingleRay(int, double, double *, double *, 
	double *, int *, int *, double *);
#endif
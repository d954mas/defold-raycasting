#ifndef NATIVE_RAYCASTING
#define NATIVE_RAYCASTING
#include <dmsdk/sdk.h>

void updateCamera(double, double, double);
void setBuffer(int, int, dmScript::LuaHBuffer*);
void setMap(lua_State*);
void castSingleRay(double, double *, double *, 
	double *, int *, int *, double *);
void updatePlane(int, int, int, int);
#endif
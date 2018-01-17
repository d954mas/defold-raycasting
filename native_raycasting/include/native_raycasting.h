#pragma once
#include <dmsdk/sdk.h>

void updateCamera(double, double, double);
void setCameraFov(double);
void setBuffer(int, int, dmScript::LuaHBuffer*);
void clearBuffer();
void setMap(lua_State*);
void updatePlane(int, int, int, int);
void castSingleRay(double, double *, double *, 
	double *, int *, int *, double *);
void castRays();
void loadTexture(dmScript::LuaHBuffer*, int);
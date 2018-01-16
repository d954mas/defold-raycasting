#pragma once
#include <dmsdk/sdk.h>
struct Camera {
	double x;
	double y;
	double angle;
	double fov;
};

struct Map {
	int width;
	int height;
	int **walls;
	int **floors;
	int **ceils;
};

struct Plane {
	int x;
	int y;
	int endX;
	int endY;
	int width;
	int height;
};

void updateCamera(struct Camera*, double, double, double);
void setCameraFov(struct Camera*, double);
void updatePlane(struct Plane*, int, int, int, int);
void parseMap(lua_State*, struct Map*);

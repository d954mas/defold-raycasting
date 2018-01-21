#pragma once
#include <dmsdk/sdk.h>
struct Camera {
	double x;
	double y;
	double angle;
	double fov=3.1415/2.0;
};

struct Map {
	int width;
	int height;
	int **walls;
	int **floors;
	int **ceils;
};

struct Plane {
	int x; //inclusicve
	int y; //inclusicve
	int endX; //inclusicve
	int endY; //inclusicve
	int width;
	int height;
	int halfHeight;
};

void updateCamera(struct Camera*, double, double, double);
void setCameraFov(struct Camera*, double);
void updatePlane(struct Plane*, int, int, int, int);
void parseMap(lua_State*, struct Map*);

#ifndef WORLD_STRUCTURES
#define WORLD_STRUCTURES
#include <dmsdk/sdk.h>
struct Camera {
	double x;
	double y;
	double angle;
};

struct Map {
	int width;
	int height;
	int **walls;
	int **floors;
	int **ceils;
};

struct Color {
	int r;
	int g;
	int b;
};

void updateCamera(struct Camera*, double, double, double);
void parseMap(lua_State*, struct Map*);

#endif
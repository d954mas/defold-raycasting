#include "world_structures.h"
#include "arrays.h"
#include <stdlib.h>
#include <dmsdk/sdk.h>

int** parseWalls(lua_State* L, int width, int height){
	int **walls = createTwoArrayInt(width, height);
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			lua_rawgeti(L,-1,y + 1);
			lua_rawgeti(L,-1,x + 1);
			int cell = (int)lua_tonumber(L, -1);
			lua_pop(L, 2);
			walls[y][x] = cell;
		}
	}
	return walls;
}

void clearMap(struct Map* map){
	clearArrayInt(map->walls, map->height);
	clearArrayInt(map->floors, map->height);
	clearArrayInt(map->ceils, map->height);
}

void updateCamera(struct Camera* camera, double x, double y, double angle){
	camera->x = x;
	camera->y = y;
	camera->angle = angle;
}

void setCameraFov(struct Camera* camera, double fov){
	camera->fov = fov;
}

void updatePlane(struct Plane* plane, int x, int y, int endX, int endY){
	plane->x = x;
	plane->y = y;
	plane->endX = endX;
	plane->endY = endY;
	plane->width = endX - x;
	plane->height = endY - y;
}

void parseMap(lua_State* L, struct Map* map){
	lua_getfield(L, 1, "WIDTH");
	lua_getfield(L, 1, "HEIGHT");
	int width = lua_tonumber(L, -2);
	int height = lua_tonumber(L, -1);
	lua_pop(L, 2);
	clearMap(map);
	map->width = width;
	map->height = height;
	lua_getfield(L, 1, "MAP");
	map->walls = parseWalls(L, width, height);
	lua_pop(L, 1);
	lua_getfield(L, 1, "FLOORS");
	map->floors = parseWalls(L, width, height);
	lua_pop(L, 1);
	lua_getfield(L, 1, "CEILS");
	map->ceils = parseWalls(L, width, height);
	lua_pop(L, 1);
}
#include <math.h>
#include "raycasting.h"
#include "world_structures.h"

//some problems here.Looks like mapX and mapY wrong.
static inline void countStep(int mapX, double startX, double dx, double absDx, int* stepX, double* sx){
	if (dx>0){
		*sx = (mapX - startX + 1) * absDx;
		*stepX = 1;
	}else{
		*sx = (startX  - mapX) * absDx;
		*stepX = - 1;
	}
}

void castRay(struct Camera* camera, int** walls, double rayAngle
	,double *perpDist, double *endX,  double *endY, int *mapXResult, int *mapYResult, double *textureX){
		double angle = camera->angle + rayAngle;
	int mapX = (int)camera->x, mapY = (int)camera->y;
	double angleSin = sin(angle), angleCos = cos(angle);
	double dx = 1.0/angleSin, dy = 1.0/angleCos;
	double absDx = fabs(dx), absDy = fabs(dy);
	double sx, sy;
	int stepX, stepY;
	countStep(mapX, camera->x, dx, absDx, &stepX, &sx);
	countStep(mapY, camera->y, dy, absDy, &stepY, &sy);
	bool hitX = true;
	while (true)
	{	
		if(sx < sy){
			mapX = mapX + stepX;
			sx = sx + absDx;
			hitX = true;
		}else{
			mapY = mapY + stepY;
			sy = sy + absDy;
			hitX = false;
		}
		if(walls[mapY][mapX] > 0){
			double dist; 
			if(hitX){
				sx = sx - absDx; //remove last dx
				dist = sx;
			}else{
				sy = sy - absDy; //remove last dy
				dist = sy;
			}
			*endX = camera->x + dist * angleSin;
			*endY = camera->y + dist * angleCos;
			if(hitX){
				*textureX = *endY;
				*textureX -= (int)*textureX;
			}else{
				*textureX = *endX;
				*textureX -= (int)*textureX;
			}
			*perpDist = dist *  cos(rayAngle);
			*mapXResult = mapX;
			*mapYResult = mapY;
			return;
		}
		
	}
} 
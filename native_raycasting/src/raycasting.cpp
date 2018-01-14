#include <math.h>
#include "raycasting.h"
#include "world_structures.h"

static void countStep(int mapX, double startX, double dx, double absDx, int* stepX, double* sx){
	if (dx>0){
		*sx = (mapX - startX + 1) * absDx;
		*stepX = 1;
	}else{
		*sx = (startX  - mapX) * absDx;
		*stepX = - 1;
	}
}

void castRay(struct Camera* camera, int** walls, double rayAngle
	,double *perpDist, double *catetX,  double *catetY, int *mapXResult, int *mapYResult, double *textureX){
	double angle = camera->angle;
	int mapX = (int)camera->x, mapY = (int)camera->y;
	double angleSin = sin(angle + rayAngle), angleCos = cos(angle + rayAngle);
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
			*catetX = dist * angleSin;
			*catetY = dist * angleCos;
			double n;
			if(hitX){
				*textureX = modf(camera->x + *catetY,&n);
			}else{
				*textureX = modf(camera->y + *catetX,&n);
			}
			*perpDist = dist *  cos(rayAngle);
			*mapXResult = mapX;
			*mapYResult = mapY;
			return;
		}
	}
} 
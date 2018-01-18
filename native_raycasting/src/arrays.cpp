#include <stdlib.h>
#include "texture.h"
int ** createTwoArrayInt(int width, int height){
	int **array = (int**)malloc(height * sizeof(int*));
	for (int i = 0; i<height; i++) 
	{
		array[i] = (int*)malloc(sizeof(int) * width);
	}
	return array;
}

int clearArrayInt(int **array, int height){
	for (int i = 0; i < height; i++)
	{
		free(array[i]);
	}
	free(array);
	return 0;
}

Color ** createTwoArrayColor(int width, int height){
	Color **array = (Color**)malloc(height * sizeof(Color*));
	for (int i = 0; i<height; i++) 
	{
		array[i] = (Color*)malloc(sizeof(Color) * width);
	}
	return array;
}

void clearArrayColor(Color **array, int height){
	for (int i = 0; i < height; i++)
	{
		free(array[i]);
	}
	free(array);
}
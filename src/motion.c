#include "motion.h"
#include <math.h>

int delta_angle(int * angle_1, int * angle_2)
{
	// Si signes opposés
	if((*angle_1) * (*angle_2) < 0)
	{
		return (ABS(*angle_1) + ABS(*angle_2));
	}else{
		return ABS((*angle_1) - (*angle_2));
	}
}

int distance(byte x, byte y)
{
	return sqrt(x*x + y*y);
}



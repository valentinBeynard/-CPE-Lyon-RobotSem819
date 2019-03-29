#include "motion.h"
#include <math.h>


int angle_distance_target(int * angle_robot, byte x_target, byte y_target)
{
	float f_angle = 0;
	int angle = 0;
	
	f_angle = atan((x_target/(float)y_target));
	f_angle = (180 * f_angle) / M_PI; 
	
	angle = (int)f_angle;
	
	return angle;
}


int delta_angle(int * current_angle, int * angle_2)
{
	int angle = 0;
	
	// Si signes opposés
	if((*current_angle) * (*angle_2) < 0)
	{
		angle = (ABS(*current_angle) + ABS(*angle_2));
	}else{
		angle = ABS((*current_angle) - (*angle_2));
	}
	
	if(current_angle <= 0)
	{
		return (-1) * angle;
	}
	else
	{
		return angle;
	}
}

float distance(byte x, byte y)
{
	
	return sqrt(x*x + y*y);
}



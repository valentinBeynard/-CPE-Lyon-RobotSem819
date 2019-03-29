#include "motion.h"
#include <math.h>


int angle_target(int * angle_robot, byte x_target, byte y_target)
{
	float f_angle = 0;
	int angle = 0;
	
	f_angle = atan((x_target/(float)y_target));
	f_angle = (180 * f_angle) / M_PI; 
	
	angle = (int)f_angle;
	
	return angle;
}


int delta_angle(int * alpha, int * beta)
{
	int angle = 0;
	
	/*
	// Si signes opposés
	if((*alpha) * (*beta) < 0)
	{
		angle = (ABS(*alpha) + ABS(*beta));
	}else{
		angle = ABS((*alpha) - (*beta));
	}
	
	if(alpha <= 0)
	{
		return (-1) * angle;
	}
	else
	{
		return angle;
	}*/
	
		// Si signes opposés
	if((*alpha) * (*beta) < 0)
	{
		if((*alpha) < (*beta))
		{
			if((*beta) < (*alpha +180))
			{
				angle = (ABS(*alpha) + ABS(*beta));
			}
			else{
				angle = (-1) * (360 - (ABS(*alpha) + ABS(*beta)));
			}
		}
		else
		{
			if((*beta) < (*alpha - 180))
			{
				angle = (-1) * (ABS(*alpha) + ABS(*beta));
			}
			else{
				angle = (360 - (ABS(*alpha) + ABS(*beta)));
			}
		}	
	}
	else{
		if((*alpha) < (*beta))
		{
			angle = ABS((*alpha) - (*beta));
		}
		else
		{
			angle = (-1) * ABS((*alpha) - (*beta));
		}
	}
	
	return angle;
}

float distance(byte x, byte y)
{
	
	return sqrt(x*x + y*y);
}



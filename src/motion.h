#ifndef MOTION_H

#define MOTION_H

#define byte unsigned char

#define ABS(X)	(X >= 0 ? X : (-1) * X)

#define M_PI 3.14159265358979323846


int angle_target(int * angle_robot, byte x_target, byte y_target);
int delta_angle(int * angle_1, int * angle_2);
float distance(byte x, byte y);

#endif
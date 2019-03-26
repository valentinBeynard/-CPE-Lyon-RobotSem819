#ifndef MOTION_H

#define MOTION_H

#define byte unsigned char

#define ABS(X)	(X >= 0 ? X : (-1) * X)

int delta_angle(int * angle_1, int * angle_2);
int distance(byte x, byte y);

#endif
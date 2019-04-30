#ifndef POWER_SENSOR_H

#define POWER_SENSOR_H

#include "c8051F020.h"
#include "pc_cmd.h"


int Mesure_Courant(char* commande);

void power_sensor_process(OUT_M1 * cmd);


#endif
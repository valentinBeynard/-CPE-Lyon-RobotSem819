#ifndef POWER_SENSOR_H

#define POWER_SENSOR_H

#include "c8051F020.h"
#include "pc_cmd.h"
#include "adc.h"

/* Time step between to energy calculation in second */
#define PS_TIME_STEP	0.010

/* CONVERSION_RATE = Vref / pow(2,10) */
#define CONVERSION_RATE	0.00234375

/* VOLTAGE_2_CURRENT_RATE = 1000 * Rshunt */
/* Ratio to convert V to mA */
#define VOLTAGE_2_CURRENT_RATE 50

/* Voltage Supply for the Robot */
#define ROBOT_BATTERY_VOLTAGE	9.6	

void Init_power_sensor();
void ps_init_timer1();

int shunt_measure();

void power_sensor_process(OUT_M1 * cmd, IN_M1 * info);

#endif
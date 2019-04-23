#ifndef POINTER_LUNIMEUX_H

#define POINTER_LUNIMEUX_H

#include "src/slave_cmd.h"

void init_timer4();
void timer_4_int();
void init_interrupt();
void init(); 
int change_lumiere(double t_allume, double t_eteint, int intensite, int N, enum Lumiere * flag_lum);

#endif;
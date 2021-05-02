#ifndef _aicar_init_h
#define _aicar_init_h
#include "common.h"

#define BEEP_PIN   B11       //¶¨Òå·äÃùÆ÷Òı½Å

extern int32 left_motor, right_motor;
extern uint16 bb_time;
extern int16 encoder1,encoder2;
extern uint16 encoder_str[];
extern int16 aim_speed;
extern uint32 servo_duty;

void aicar_init();

#endif
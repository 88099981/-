#ifndef _aicar_motor_h
#define _aicar_motor_h
#include "common.h"

#define DIR_1 D0
#define DIR_2 D1
#define PWM_1 PWM2_MODULE3_CHA_D2
#define PWM_2 PWM2_MODULE3_CHB_D3

void aicar_motor_init(void);
void aicar_motor_control(int32 motor1, int32 motor2);

#endif


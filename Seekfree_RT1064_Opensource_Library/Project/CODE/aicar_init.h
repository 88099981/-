#ifndef _aicar_init_h
#define _aicar_init_h
#include "common.h"

#define BEEP_PIN   B11       //¶¨Òå·äÃùÆ÷Òý½Å
#define EPSOLON 1e-7
#define SPEED_SET 70

typedef enum
{
    HOLDER_LEFT,
    HOLDER_RIGHT,
    SERVO_LEFT,
    SERVO_RIGHT,
    ANIMAL,
    FRUIT
}KIND;

extern vuint8 zuo_yuanhuan_flag;
extern vuint8 you_yuanhuan_flag;
extern int16 left_motor, right_motor;
extern uint16 bb_time;
extern int16 encoder1,encoder2;
extern uint16 encoder_str[];
extern int16 aim_speed_set;
extern int16 aim_speed;
extern uint32 servo_duty;
extern vuint8 break_flag;
extern vuint8 adc_isr_enable;
extern uint16 stop_cnt;
extern int32 turn_sum;
extern float chasu_k,chasu_b;
extern vuint8 magic_mode;
extern uint8 magic_data[];
extern uint16 use_time;

void aicar_init();

#endif
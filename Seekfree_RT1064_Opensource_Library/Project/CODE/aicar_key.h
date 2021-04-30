#ifndef _aicar_key_h
#define _aicar_key_h

#include"common.h"
#include "SEEKFREE_FONT.h"

#define KEY1    C31
#define KEY2    C27
#define KEY3    C26
#define KEY4    C4
//定义拨码开关引脚
#define SW1     D4
#define SW2     D27


//拨码开关状态变量
extern uint8 sw1_status;
extern uint8 sw2_status;

//开关状态变量
extern uint8 key1_status;
extern uint8 key2_status;
extern uint8 key3_status;
extern uint8 key4_status;

//上一次开关状态变量
extern uint8 key1_last_status;
extern uint8 key2_last_status;
extern uint8 key3_last_status;
extern uint8 key4_last_status;

//开关标志位
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;

void aicar_key_init();
void aicar_switch_init();
void aicar_key_get();
void aicar_switch_get();

#endif
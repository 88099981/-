#ifndef _aicar_element_h
#define _aicar_element_h
#include"common.h"
#define HD_IN_DELAY 50
#define HD_OUT_DELAY 90
#define HD_IN 10
#define HD_OUT 10
#define HD_TURN 20

extern uint8 hd_in;
extern uint8 hd_out;
extern uint8 hd_in_delay;
extern uint8 hd_out_delay;
extern uint8 hd_turn;
extern vuint8 ruhuan_turn;

//º¯Êý
void aicar_huandao();

#endif
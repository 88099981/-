#ifndef _aicar_meau_h
#define _aicar_meau_h
#include "common.h"

extern uint8 meau_page;
extern uint8 pointer_page;
extern uint8 pointer_arrow;

typedef enum
{
    MEAU_MAIN,
    MEAU_DEBUG,//无显示蓝牙传图像
    MEAU_DEBUG_0,//摄像头显示图像
    MEAU_DEBUG_1,//电感显示值
    MEAU_DEBUG_2,//无显示蓝牙传图像
    MEAU_DEBUG_3,//舵机pid
    MEAU_DEBUG_4,//电机pid
    MEAU_DEBUG_5,//环岛
    MEAU_DEBUG_6,//差速
    MEAU_PARA,//参数显示
    MEAU_PARA_0,//环岛参数
    MEAU_PARA_0_0,//ruhuan
    MEAU_PARA_0_1,//chuhuan
    MEAU_PARA_0_2,//ruhuan_delay
    MEAU_PARA_0_3,//chuhuan_delay
    MEAU_PARA_0_4,//ruhuan_turn
    MEAU_GOGOGO,
    MEAU_GOGOGO_0,//mix
    MEAU_GOGOGO_1,//ad
    MEAU_GOGOGO_2,//cam
    MEAU_OURTEAM,
    MEAU_NON,//不显示
    
}MEAU_NUM;

void aicar_meau();
void show_page(uint8);
void show_arrow(uint8);
void debug_camera();
void debug_adc();
void debug_wireless();
void debug_servo();
void debug_motor();
void debug_huandao();
void debug_chasu();
void gogogo_camera();
void gogogo_adc();
void gogogo_mix();
void para_huandao();

#endif
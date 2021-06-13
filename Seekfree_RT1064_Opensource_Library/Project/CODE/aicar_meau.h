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
    MEAU_PARA,//参数显示
    MEAU_GOGOGO,
    MEAU_GOGOGO_0,
    MEAU_GOGOGO_1,
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
void gogogo_camera();
void gogogo_adc();

#endif
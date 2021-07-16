#ifndef _aicar_meau_h
#define _aicar_meau_h
#include "common.h"

extern uint8 meau_page;
extern uint8 pointer_page;
extern uint8 pointer_arrow;
extern int16 sancha_wait_in;
extern vuint8 sancha_wait_banma;

typedef enum
{
    MEAU_MAIN,
    MEAU_DEBUG,//debug主菜单
    MEAU_DEBUG_0,//摄像头显示图像
    MEAU_DEBUG_1,//电感显示值
    MEAU_DEBUG_2,//无显示蓝牙传图像
    MEAU_DEBUG_3,//舵机pid
    MEAU_DEBUG_4,//电机pid
    MEAU_DEBUG_5,//环岛
    MEAU_DEBUG_6,//差速
    MEAU_DEBUG_7,//陀螺仪20602
    MEAU_DEBUG_8,//apriltags识别
    MEAU_DEBUG_9,//number识别
    MEAU_PARA,//参数显示
    MEAU_PARA_0,//环岛参数
    MEAU_PARA_0_0,//ruhuan
    MEAU_PARA_0_1,//chuhuan
    MEAU_PARA_0_2,//ruhuan_delay
    MEAU_PARA_0_3,//chuhuan_delay
    MEAU_PARA_0_4,//ruhuan_turn
    MEAU_GOGOGO,
    MEAU_GOGOGO_0,//模式选择
    MEAU_GOGOGO_1,//左右出库
    MEAU_GOGOGO_2,//magic
    MEAU_GOGOGO_3,//apriltag
    MEAU_GOGOGO_4,//number
    MEAU_GOGOGO_5,//animal_fruit
    MEAU_GOGOGO_6,//GOGOGO!
    MEAU_OURTEAM,
    MEAU_NON,//不显示
    
}MEAU_NUM;

typedef enum
{
    GOGOGO_ADC,//电感模式
    GOGOGO_CAM,//摄像头模式
    GOGOGO_MIX,//混合模式
    GOGOGO_MAGIC,//神奇模式
    GOGOGO_NON//不显示
}RUN_MODE;

typedef enum
{
    GARAGE_LEFT,//左出库
    GARAGE_RIGHT,//右出库
    GARAGE_NON,//不出库
}GARAGE_MODE;
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
void debug_20602();
void debug_apriltag();
void debug_number();
void gogogo_camera();
void gogogo_adc();
void gogogo_mix();
void gogogo_choose();
void gogogo_run();
void para_huandao();

#endif
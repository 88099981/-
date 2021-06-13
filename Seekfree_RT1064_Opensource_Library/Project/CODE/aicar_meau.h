#ifndef _aicar_meau_h
#define _aicar_meau_h
#include "common.h"

extern uint8 meau_page;
extern uint8 pointer_page;
extern uint8 pointer_arrow;

typedef enum
{
    MEAU_MAIN,
    MEAU_DEBUG,//����ʾ������ͼ��
    MEAU_DEBUG_0,//����ͷ��ʾͼ��
    MEAU_DEBUG_1,//�����ʾֵ
    MEAU_DEBUG_2,//����ʾ������ͼ��
    MEAU_DEBUG_3,//���pid
    MEAU_DEBUG_4,//���pid
    MEAU_PARA,//������ʾ
    MEAU_GOGOGO,
    MEAU_GOGOGO_0,
    MEAU_GOGOGO_1,
    MEAU_OURTEAM,
    MEAU_NON,//����ʾ
    
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
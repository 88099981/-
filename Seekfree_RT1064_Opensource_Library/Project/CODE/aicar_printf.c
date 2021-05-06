/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã� printf                       00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_printf.h"
#include "aicar_init.h"
#include "aicar_adc.h"
#include "aicar_servo.h"
#include "SEEKFREE_18TFT.h"
#include "aicar_key.h"
#include "aicar_motor.h"
#include "aicar_error.h"
void aicar_adc_printf()
{
    lcd_showstr(0,1,"ad_value1:");    
    lcd_showuint16(12*8,1,ad_value1);
    lcd_showstr(0,2,"ad_value2:");
    lcd_showuint16(12*8,2,ad_value2);
    lcd_showstr(0,3,"ad_value3:");
    lcd_showuint16(12*8,3,ad_value3);
    lcd_showstr(0,4,"ad_value4:");
    lcd_showuint16(12*8,4,ad_value4);
    lcd_showstr(0,5,"ad_value5:");
    lcd_showuint16(12*8,5,ad_value5);
    lcd_showstr(0,6,"ad_value6:");
    lcd_showuint16(12*8,6,ad_value6);
    lcd_showstr(0,7,"ad_all:");
    lcd_showuint16(12*8,7,ad_value_all);
}

void aicar_servopid_printf()
{
    lcd_showstr(0,0,"ad_all:");
    lcd_showuint16(12*8,0,ad_value_all);
    lcd_showstr(0,1,"ad_left:");    
    lcd_showfloat(12*8,1,ad_left,3,2);
    lcd_showstr(0,2,"ad_right:");
    lcd_showfloat(12*8,2,ad_right,3,2);
    lcd_showstr(0,3,"ad_error:");
    lcd_showfloat(12*8,3,ad_error,3,2);
    lcd_showstr(0,4,"angle:");
    lcd_showint16(12*8,4,servo_angle);
    lcd_showstr(0,5,"bk_flag:");
    lcd_showuint8(12*8,5,break_flag);
    lcd_showstr(0,6,"kp_ad:");
    lcd_showfloat(12*8,6,kp_ad,3,2);
    lcd_showstr(0,7,"kd_ad:");
    lcd_showfloat(12*8,7,kd_ad,3,2);
}
void aicar_motorpid_printf()
{
    lcd_showstr(0,0,"encoder1:");  
    lcd_showint16(12*8,0,encoder1);
    lcd_showstr(0,1,"encoder2:");  
    lcd_showint16(12*8,1,encoder2);
    lcd_showstr(0,2,"aim_speed:");  
    lcd_showint32(12*8,2,aim_speed, 5);
    lcd_showstr(0,3,"sw1_status:");
    lcd_showuint8(12*8,3,sw1_status);    
    lcd_showstr(0,4,"sw2_status:");  
    lcd_showuint8(12*8,4,sw2_status);        
    lcd_showstr(0,5,"kp_l:");  
    lcd_showfloat(12*8,5,kp_l, 3,2);
    lcd_showstr(0,6,"ki_l:");  
    lcd_showfloat(12*8,6,ki_l, 3,2);
    lcd_showstr(0,7,"speed1:");  
    lcd_showfloat(12*8,7,speed_out1, 5,0);
}
void aicar_chasu_printf()
{
    lcd_showstr(0,0,"ad_left:");    
    lcd_showfloat(12*8,0,ad_left,3,2);
    lcd_showstr(0,1,"ad_right:");
    lcd_showfloat(12*8,1,ad_right,3,2);
    lcd_showstr(0,2,"ad_error:");
    lcd_showfloat(12*8,2,ad_error,3,2);
    lcd_showstr(0,3,"angle:");
    lcd_showint16(12*8,3,servo_angle);
    lcd_showstr(0,4,"chasu_k:");    
    lcd_showfloat(12*8,4,chasu_k,3,2);
    lcd_showstr(0,5,"chasu_b:");
    lcd_showfloat(12*8,5,chasu_b,3,2);
    lcd_showstr(0,6,"left:");
    lcd_showint16(12*8,6,left_motor);
    lcd_showstr(0,7,"right:");
    lcd_showint16(12*8,7,right_motor);
    
}

void aicar_camerapid_printf()
{
    lcd_showstr(0,2,"cam_err:");
    lcd_showint8(12*8,2,camera_error);
    lcd_showstr(0,5,"a_cam:");
    lcd_showfloat(12*8,5,a_cam,1,4);
    lcd_showstr(0,6,"kp_cam:");
    lcd_showfloat(12*8,6,kp_cam,3,2);
    lcd_showstr(0,7,"kd_cam:");
    lcd_showfloat(12*8,7,kd_cam,3,2);
}
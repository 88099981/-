/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã�  ��init                      00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_init.h"
#include "zf_pwm.h"
#include "aicar_servo.h"
#include "zf_gpio.h"
#include "SEEKFREE_MT9V03X_CSI.h"
#include "aicar_motor.h"
#include "SEEKFREE_18TFT.h"
#include "zf_qtimer.h"
#include "SEEKFREE_WIRELESS.h"

int32 left_motor=0,right_motor=0;
int16 aim_speed=0;
uint16 bb_time=0;
int16 encoder1=0,encoder2=0;
uint16 encoder_str[20]={0};

void aicar_init()
{
    mt9v03x_csi_init();//����ͷ
    //adc
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);//encoder
    lcd_init();//lcd
    seekfree_wireless_init();//wireless
    gpio_init(BEEP_PIN,GPO,0,GPIO_PIN_CONFIG);//bb
    aicar_servo_init();//servo
    aicar_motor_init();//motor
}
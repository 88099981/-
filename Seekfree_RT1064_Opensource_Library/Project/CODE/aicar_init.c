/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用：  总init                      00**
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
#include "aicar_key.h"
#include "aicar_adc.h"

vuint8 zuo_yuanhuan_flag=0;
vuint8 you_yuanhuan_flag=0;

vuint8 break_flag=0;
vuint8 adc_isr_enable=0;
uint16 stop_cnt=0;
int16 left_motor=0,right_motor=0;
uint32 servo_duty=0;
int16 aim_speed=0;//pid用,最大值不是50000，别胡改
uint16 bb_time=0;
int16 encoder1=0,encoder2=0;
uint16 encoder_str[20]={0};

double chasu_k=0.0,chasu_b=0.0;

void aicar_init()
{
    mt9v03x_csi_init();//摄像头
    aicar_adc_init();//adc
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);//encoder
    aicar_key_init();//key
    aicar_switch_init();//switch
    lcd_init();//lcd
    seekfree_wireless_init();//wireless
    gpio_init(BEEP_PIN,GPO,0,GPIO_PIN_CONFIG);//bb
    aicar_servo_init();//servo
    aicar_motor_init();//motor
}
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
#include "aicar_key.h"
#include "aicar_adc.h"
#include "aicar_servo.h"
#include "aicar_motor.h"
#include "aicar_flash.h"
#include "aicar_uart.h"
#include "zf_flash.h"
#include "zf_pwm.h"
#include "zf_qtimer.h"
#include "zf_gpio.h"
#include "SEEKFREE_MT9V03X_CSI.h"
#include "SEEKFREE_ICM20602.h"
#include "SEEKFREE_18TFT.h"
#include "SEEKFREE_WIRELESS.h"



vuint8 zuo_yuanhuan_flag=0;//��Բ����־λ
vuint8 you_yuanhuan_flag=0;//��Բ����־λ
vuint8 break_flag=0;//ͣ����־λ
vuint8 adc_isr_enable=0;//���debugʹ�õĵ���ܳ���־λ
vuint8 magic_mode=0;//��ѡģʽ
uint16 stop_cnt=0;//ͣ����λ������
uint16 use_time=0;//��ʱ����
uint16 bb_time=0;//bb
int16 left_motor=0,right_motor=0;
int16 aim_speed=0;//pid��,���ֵ����50000�������
int16 encoder1=0,encoder2=0;
uint32 servo_duty=0;
int32 turn_sum=0;//����ת��

uint8 magic_data[5]={0};//��ѡģʽ����
uint16 encoder_str[20]={0};

float chasu_k=0.0,chasu_b=0.0;//��������

void aicar_init()
{
    mt9v03x_csi_init();//����ͷ
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
    flash_init();   //��ʼ��flash
    aicar_flash_read();//��ȡflash
    icm20602_init_spi();//20602    
    aicar_uart_init();
    aicar_holder_init();//��̨
    aicar_laser_init();//������
}
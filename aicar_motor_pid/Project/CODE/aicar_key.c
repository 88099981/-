/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã�  key                         00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "SEEKFREE_FONT.h"
#include "aicar_key.h"
#include "SEEKFREE_18TFT.h"
#include "zf_gpio.h"
#include "zf_systick.h"
#include "aicar_init.h"
#include "aicar_motor.h"

uint8 sw1_status;
uint8 sw2_status;

//����״̬����
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//��һ�ο���״̬����
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

//���ر�־λ
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

void aicar_key_init()
{
    gpio_init(KEY1,GPI,0,GPIO_PIN_CONFIG);
    gpio_init(KEY2,GPI,0,GPIO_PIN_CONFIG);
    gpio_init(KEY3,GPI,0,GPIO_PIN_CONFIG);
    gpio_init(KEY4,GPI,0,GPIO_PIN_CONFIG);
}

void aicar_switch_init()
{
    gpio_init(SW1,GPI,0,GPIO_PIN_CONFIG);
    gpio_init(SW2,GPI,0,GPIO_PIN_CONFIG);//12ѧϰ���Ϸ���
}

void aicar_key_get()
{

    aicar_switch_get();

    key1_last_status = key1_status;
    key2_last_status = key2_status;
    key3_last_status = key3_status;
    key4_last_status = key4_status;
   //��ȡ��ǰ����״̬
    key1_status = gpio_get(KEY1);
    key2_status = gpio_get(KEY2);
    key3_status = gpio_get(KEY3);
    key4_status = gpio_get(KEY4);
        
    //��⵽��������֮��  ���ſ���λ��־λ
    if(key1_status && !key1_last_status)    key1_flag = 1;
    if(key2_status && !key2_last_status)    key2_flag = 1;
    if(key3_status && !key3_last_status)    key3_flag = 1;
    if(key4_status && !key4_last_status)    key4_flag = 1;
        
    //��־λ��λ֮�󣬿���ʹ�ñ�־λִ���Լ���Ҫ�����¼�
    if(key1_flag)   
    {
        if(sw1_status)
        {
            key1_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            aim_speed+=30;
        }
        else if(sw2_status)
        {
            key1_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            kp_l+=1.0;
        }
    }
                
    if(key2_flag)       
    {           
        if(sw1_status)
        {
            key2_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            aim_speed=0;
        }
        else if(sw2_status)
        {
            key2_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            kp_l-=1.0;
        }
             
    }
        
    if(key3_flag)           
    {
        if(sw1_status)
        {
            key3_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            aim_speed-=30;
        }
        else if(sw2_status)
        {
            key3_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            ki_l+=1.0;
        }
    }
        
    if(key4_flag)   
    {
        if(sw1_status)
        {
            key4_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            bb_time=10;
        }
        else if(sw2_status)
        {
            key4_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
            ki_l-=1.0;
        }
    }
             

}
    
void aicar_switch_get()
{
     sw1_status = gpio_get(SW1);
     sw2_status = gpio_get(SW2);
}


/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã� motor                      00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_motor.h"
#include "zf_pwm.h"
#include "zf_gpio.h"

void aicar_motor_init()
{
    gpio_init(DIR_1, GPO, 0, GPIO_PIN_CONFIG); 		//��Ƭ���˿�D0 ��ʼ��DIR_1			GPIO
    gpio_init(DIR_2, GPO, 0, GPIO_PIN_CONFIG); 		//��Ƭ���˿�D1 ��ʼ��DIR_2			GPIO
    pwm_init(PWM_1, 17000, 0);      				//��Ƭ���˿�D2 ��ʼ��PWM_1����10K ռ�ձ�0
    pwm_init(PWM_2, 17000, 0);     					//��Ƭ���˿�D3 ��ʼ��PWM_2����10K ռ�ձ�0
}

void aicar_motor_control(int32 motor1, int32 motor2)//1��2��,���Ʒ�����hcarһ��
{ 
    motor1=-motor1;
    if (motor1 > 0)
    {
        gpio_set(DIR_1, 0);
        pwm_duty(PWM_1, motor1);
    }
    else
    {
        gpio_set(DIR_1, 1);
        pwm_duty(PWM_1, PWM_DUTY_MAX+motor1);
    }

    if (motor2 >= 0)
    {
        gpio_set(DIR_2, 0);
        pwm_duty(PWM_2, motor2);
    }
    else
    {
        gpio_set(DIR_2, 1);
        pwm_duty(PWM_2, PWM_DUTY_MAX+motor1);
    }

}

void aicar_motor_pid()
{
    
}

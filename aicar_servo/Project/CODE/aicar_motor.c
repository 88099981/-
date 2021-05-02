/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用： motor                      00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_motor.h"
#include "zf_pwm.h"
#include "zf_gpio.h"

void aicar_motor_init()
{
    gpio_init(DIR_1, GPO, 0, GPIO_PIN_CONFIG); 		//单片机端口D0 初始化DIR_1			GPIO
    gpio_init(DIR_2, GPO, 0, GPIO_PIN_CONFIG); 		//单片机端口D1 初始化DIR_2			GPIO
    pwm_init(PWM_1, 17000, 0);      				//单片机端口D2 初始化PWM_1周期10K 占空比0
    pwm_init(PWM_2, 17000, 0);     					//单片机端口D3 初始化PWM_2周期10K 占空比0
}

void aicar_motor_control(int32 motor1, int32 motor2)//1左2右,控制方法和hcar一样
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

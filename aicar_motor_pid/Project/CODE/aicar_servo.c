/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用： servo                        00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_init.h"
#include "aicar_servo.h"
#include "zf_pwm.h"

void aicar_servo_init(void)
{
    pwm_init(S_MOTOR1_PIN,50,1.5*50000/20);
}
void aicar_servo_control(uint32 duty)//前轮舵机，3450-4250，中值3850
{
    if(duty<=0)
    {
        duty=0;
    }
    else if(duty>PWM_DUTY_MAX)
    {
        duty=PWM_DUTY_MAX;
    }
    pwm_duty(S_MOTOR1_PIN,duty);
}
void aicar_servo_holder(void)//云台，1150-6350，中值3850
{
    
}
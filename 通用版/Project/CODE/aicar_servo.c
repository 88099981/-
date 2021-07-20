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
    pwm_init(S_MOTOR1_PIN,50,3750);
}
void aicar_servo_control(uint32 duty)//前轮舵机，3450-4250，中值3850
{
    if(duty<=3450)
    {
        duty=3450;
    }
    else if(duty>=4250)
    {
        duty=4250;
    }
    pwm_duty(S_MOTOR1_PIN,duty);
}
void aicar_holder_init(void)//云台，1150-6350，中值3850
{
    pwm_init(S_MOTOR2_PIN,50,3850);
}
void aicar_holder_control(uint32 duty)//云台，1150-6350，中值3850
{
    if(duty<=1150)
    {
        duty=1150;
    }
    else if(duty>=6350)
    {
        duty=6350;
    }
    pwm_duty(S_MOTOR2_PIN,duty);
}
void aicar_laser_init(void)
{
    pwm_init(S_MOTOR3_PIN,125,0);//激光器频率为125hz，占空比为50%
}
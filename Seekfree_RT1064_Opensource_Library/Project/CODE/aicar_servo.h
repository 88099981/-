#ifndef _aicar_servo_h
#define _aicar_servo_h

#define S_MOTOR1_PIN   PWM4_MODULE2_CHA_C30       //定义舵机引脚

//函数
void aicar_servo_init(void);
void aicar_servo_control(uint32 duty);
void aicar_servo_print(void);
#endif
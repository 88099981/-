#ifndef _aicar_servo_h
#define _aicar_servo_h

#define S_MOTOR1_PIN    PWM4_MODULE2_CHA_C30 //定义舵机引脚
#define S_MOTOR2_PIN    PWM1_MODULE0_CHA_D12 //定义云台引脚
#define S_MOTOR3_PIN    PWM1_MODULE1_CHA_D14 //定义激光器引脚

//函数
void aicar_servo_init(void);
void aicar_servo_control(uint32 duty);
void aicar_holder_init(void);
void aicar_holder_control(uint32 duty);
void aicar_laser_init(void);
#endif
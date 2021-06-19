#ifndef _aicar_adc_h
#define _aicar_adc_h
#include "common.h"
#include "zf_adc.h"

#define POWER_ADC1_MOD  ADC_1       //定义通道一 ADC模块号
#define POWER_ADC1_PIN  ADC1_CH3_B14//定义通道一 ADC引脚
                                                     
#define POWER_ADC2_MOD  ADC_1       //定义通道二 ADC模块号
#define POWER_ADC2_PIN  ADC1_CH4_B15//定义通道二 ADC引脚

#define POWER_ADC3_MOD  ADC_1       //定义通道三 ADC模块号
#define POWER_ADC3_PIN  ADC1_CH7_B18//定义通道三 ADC引脚            
    
#define POWER_ADC4_MOD  ADC_1       //定义通道四 ADC模块号
#define POWER_ADC4_PIN  ADC1_CH8_B19//定义通道四 ADC引脚

#define POWER_ADC5_MOD  ADC_1       //定义通道三 ADC模块号
#define POWER_ADC5_PIN  ADC1_CH10_B21//定义通道三 ADC引脚

#define POWER_ADC6_MOD  ADC_1       //定义通道四 ADC模块号
#define POWER_ADC6_PIN  ADC1_CH12_B23//定义通道四 ADC引脚


#define KP_ad_str 9.0
#define KD_ad_str 5.0
#define KP_ad_turn 1.0
#define KD_ad_turn 10.0

//变量
extern uint16  ad_value1;
extern uint16  ad_value2;
extern uint16  ad_value3;
extern uint16  ad_value4;
extern uint16  ad_value5;
extern uint16  ad_value6;
extern uint16  ad_value_all;
extern int16 ad_speed;
//extern uint16  guiyi_ad1,guiyi_ad2,guiyi_ad3,guiyi_ad4;
extern float ad_error;
extern float kp_ad, kd_ad;
extern float ad_right,ad_left;
extern float lasttime_ad;
extern float lasttime_cam;
extern int16 servo_angle;


//函数
void aicar_adc_init(void);
void aicar_adc_get(void);
void aicar_stop(void);


#endif
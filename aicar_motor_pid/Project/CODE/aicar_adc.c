/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã� adc                           00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_adc.h"


uint16  ad_value1;
uint16  ad_value2;
uint16  ad_value3;
uint16  ad_value4;
int16 ad_speed=0;
float lasttime_ad=0.0;
//uint16  guiyi_ad1,guiyi_ad2,guiyi_ad3,guiyi_ad4;
float ad_error=0.0;
float kp_ad=0.0, kd_ad=0.0;
float ad_left=0.0, ad_right=0.0;

void aicar_adc_init()
{
    adc_init(POWER_ADC1_MOD,POWER_ADC1_PIN,ADC_8BIT);  //ͬһ��ADCģ��ֱ���Ӧ������Ϊһ���ģ�������ò�һ���������һ����ʼ��ʱ���õķֱ�����Ч
    adc_init(POWER_ADC2_MOD,POWER_ADC2_PIN,ADC_8BIT);
    adc_init(POWER_ADC3_MOD,POWER_ADC3_PIN,ADC_8BIT);
    adc_init(POWER_ADC4_MOD,POWER_ADC4_PIN,ADC_8BIT);
}

void aicar_adc_get()
{
    ad_value1 = adc_mean_filter(POWER_ADC1_MOD,POWER_ADC1_PIN,10);        
    ad_value2 = adc_mean_filter(POWER_ADC2_MOD,POWER_ADC2_PIN,10);        
    ad_value3 = adc_mean_filter(POWER_ADC3_MOD,POWER_ADC3_PIN,10);        
    ad_value4 = adc_mean_filter(POWER_ADC4_MOD,POWER_ADC4_PIN,10);
}
/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用： error                       00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_error.h"
#include "aicar_init.h"
#include "include.h"
#include "aicar_adc.h"

int16 camera_error=0;
float kp_cam=0.0;
float kd_cam=0.0;
float a_cam=0.0;
void aicar_camera_error()
{
    camera_error=0;
    if(Mid_Num<=ERROR_EDGE)
    {
        for(uint8 i=1;i<=Mid_Num;i++)//遍历
        {
            camera_error+=Mid_X[i]-94;
        }
        camera_error=-camera_error/Mid_Num;
    }
    else
    {
        for(uint8 i=1;i<=ERROR_EDGE;i++)//遍历
        {
            camera_error+=Mid_X[i]-94;
        }
        camera_error=-camera_error/ERROR_EDGE;
    }
//    for(uint8 i=1;i<=Mid_Num;i++)//全场遍历
//    {
//        camera_error+=Mid_X[i]-94;
//    }
//    camera_error=-camera_error/Mid_Num;
    
//    kp_cam=camera_error*camera_error*a_cam;//二次
    servo_angle=(int16)(kp_cam*camera_error + kd_cam*(camera_error-lasttime_ad));
    lasttime_ad=camera_error;
    
    servo_duty=3850+servo_angle;
    if(servo_duty<=3450)
    {
        servo_duty=3450;
    }
    else if(servo_duty>=4250)
    {
        servo_duty=4250;
    }
}

void aicar_adc_error()
{
    ad_left=ad_value2*0.3+ad_value3*0.7;
    ad_right=ad_value4*0.7+ad_value5*0.3;
    
    ad_error=(ad_left-ad_right)*100/(ad_left+ad_right);
    servo_angle=(int16)(kp_ad*ad_error + kd_ad*(ad_error-lasttime_ad));
    lasttime_ad=ad_error;
    
    servo_duty=3850+servo_angle;
    if(servo_duty<=3450)
    {
        servo_duty=3450;
    }
    else if(servo_duty>=4250)
    {
        servo_duty=4250;
    }
}
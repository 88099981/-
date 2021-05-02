/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用：  go                          00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_gogogo.h"
#include "headfile.h"

void aicar_gogogo()
{
    DisableGlobalIRQ();
    board_init();           //务必保留，本函数用于初始化MPU 时钟 调试串口
    systick_delay_ms(1000);

    aicar_init();
    aicar_switch_get();
    pit_init();                     //初始化pit外设
    pit_interrupt_ms(PIT_CH0,10);  //初始化pit通道0 周期
    NVIC_SetPriority(PIT_IRQn,5);  //设置中断优先级 范围0-15 越小优先级越高 四路PIT共用一个PIT中断函数
    servo_duty=3750;
    EnableGlobalIRQ(0); //总中断最后开启
    while(1)
    {
//        if(mt9v03x_csi_finish_flag)
//        {      
//            mt9v03x_csi_finish_flag = 0;
//            //使用缩放显示函数，根据原始图像大小 以及设置需要显示的大小自动进行缩放或者放大显示
//            lcd_displayimage032_zoom(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 160, 128);
//      
//        }
        aicar_key_get();
        lcd_showstr(0,0,"encoder1:");  
        lcd_showint16(12*8,0,encoder1);
        lcd_showstr(0,1,"encoder2:");  
        lcd_showint16(12*8,1,encoder2);

        lcd_showstr(0,2,"servo_duty:");  
        lcd_showint32(12*8,2,servo_duty, 5);
        
        lcd_showstr(0,3,"sw1_status:");  
        lcd_showuint8(12*8,3,sw1_status);
        lcd_showstr(0,4,"sw2_status:");  
        lcd_showuint8(12*8,4,sw2_status);
        
        lcd_showstr(0,5,"left_motor:");  
        lcd_showint32(12*8,5,left_motor, 5);
        lcd_showstr(0,6,"right_motor:");  
        lcd_showint32(12*8,6,right_motor, 5);
    }
}
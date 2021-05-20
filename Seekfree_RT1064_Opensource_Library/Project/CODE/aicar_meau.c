/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用：  meau                         00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_init.h"
#include "aicar_key.h"
#include "aicar_meau.h"
#include "aicar_printf.h"
#include "aicar_adc.h"
#include "SEEKFREE_MT9V03X_CSI.h"
#include "SEEKFREE_18TFT.h"
#include "include.h"
#include "camera.h"
#include "aicar_error.h"
#include "aicar_chasu.h"
#include "aicar_wireless.h"

uint8 img[IMG_H][IMG_W];		//收到的图像
uint8 meau_page=0;
uint8 meau_last_page=0;

void aicar_meau()
{
    aicar_switch_get();
    aicar_key_get();
    if(sw1_status==0&&sw2_status==0)   //可以通过按键调节参数kp kd，可以用摄像头偏差打角
    {
        meau_last_page=meau_page;
        meau_page=1;
        if(meau_last_page!=meau_page)
        {
            lcd_clear(WHITE);  
            
        }
        break_flag=1;
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
						cut_image_to_img2();//copy
						binary_img();
						Search_main();    
        }      
        aicar_camera_error();
        aicar_camerapid_printf();
        if(key1_flag)
        {
            key1_flag=0;
            kp_cam+=0.3;
        }
        else if(key2_flag)
        {
            key2_flag=0;
            kp_cam-=0.3;
        }
        else if(key3_flag)
        {
            key3_flag=0;
            kd_cam+=0.3;
        }
        else if(key4_flag)
        {
            key4_flag=0;
            kd_cam-=0.3;
        }        
    }
                
    else if(sw1_status==0&&sw2_status==1) // 显示图像到lcd上，可以调节阈值并上传图像，可以打角      
    {           
        meau_last_page=meau_page;
        meau_page=2;
        if(meau_last_page!=meau_page)
        {
            lcd_clear(WHITE);    
        }
        break_flag=1;
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy						
            binary_img();			
            Search_main();

            lcd_displayimage032_zoom(img[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 160, 128);
      
        }        
        if(key1_flag)
        {
            key1_flag=0;
            ostu_thres+=1;
        }
        else if(key2_flag)
        {
            key2_flag=0;
            ostu_thres-=1;
        }
        else if(key3_flag)
        {
            key3_flag=0;
            //aicar_camera_wireless(USART_8, img[0], MT9V03X_CSI_W, MT9V03X_CSI_H);//山外上位机
            csi_seekfree_sendimg_03x(USART_8,img[0],MT9V03X_CSI_W,MT9V03X_CSI_H);//逐飞上位机
        }
        else if(key4_flag)
        {
            key4_flag=0;
            bb_time=10;
        }     
        aicar_camera_error();//打角
    }
        
    else if(sw1_status==1&&sw2_status==0) //摄像头跑车程序，可通过按键调节速度，已差速         
    {
        meau_last_page=meau_page;
        meau_page=3;
        if(meau_last_page!=meau_page)
        {
            lcd_clear(WHITE); 
            stop_cnt=0;
            break_flag=0;
            aim_speed=SPEED_SET;
        }
        if(key1_flag)
        {
            key1_flag=0;
            aim_speed+=10;
        }
        else if(key2_flag)
        {
            key2_flag=0;
            aim_speed-=10;
        }
        else if(key3_flag)
        {
            key3_flag=0;
            aim_speed+=5;
        }
        else if(key4_flag)
        {
            key4_flag=0;
            aim_speed-=5;
        }        
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
						cut_image_to_img2();//copy
						binary_img();
						Search_main();    
        }        
        aicar_camera_error();
        aicar_adc_get();//停车用
        aicar_chasu();
        lcd_showstr(0,2,"aim_speed:");
        lcd_showint16(12*8,2,aim_speed);
//        lcd_showstr(0,2,"bk_flag:");
//        lcd_showuint8(12*8,2,break_flag);
//        lcd_showstr(0,4,"kp_ad:");
//        lcd_showfloat(12*8,4,kp_ad,3,2);
//        lcd_showstr(0,5,"kd_ad:");
//        lcd_showfloat(12*8,5,kd_ad,3,2);
        lcd_showstr(0,6,"left:");
        lcd_showint16(12*8,6,left_motor);
        lcd_showstr(0,7,"right:");
        lcd_showint16(12*8,7,right_motor);
        //aicar_chasu_printf();
    }
    else if(sw1_status==1&&sw2_status==1)  //可以通过电感偏差跑车，并上传图像        
    {
        meau_last_page=meau_page;
        meau_page=4;
        if(meau_last_page!=meau_page)
        {
            lcd_clear(WHITE);   
            stop_cnt=0;
            break_flag=0;
            aim_speed=30;
        }
        adc_isr_enable=1;
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy						
            binary_img();			
            Search_main();
          
            //aicar_camera_wireless(USART_8, img[0], MT9V03X_CSI_W, MT9V03X_CSI_H);//山外上位机
            csi_seekfree_sendimg_03x(USART_8,img[0],MT9V03X_CSI_W,MT9V03X_CSI_H);//逐飞上位机
        }      
        aicar_n_chasu();
    }
             
}

void aicar_adc_meau1()//显示adc数值，并且打角
{
    aim_speed=0;
    aicar_adc_get();
    aicar_adc_error();
    aicar_adc_printf();
}
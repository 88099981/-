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
#include "camera.h"
#include "aicar_error.h"
#include "aicar_chasu.h"
#include "aicar_wireless.h"
#include "aicar_element.h"
#include "fd_GetMid.h"

//uint8 img[IMG_H][IMG_W];		//收到的图像
uint8 meau_page=0;
uint8 meau_last_page=0;


void show_page(uint8 page)
{
    switch(page)
    {
    case MEAU_MAIN:print_main();break;
    case MEAU_DEBUG:print_debug();break;
    case MEAU_PARA:print_parameter();break;
    case MEAU_PARA_0:para_huandao();break;
    case MEAU_PARA_0_0:para_huandao();break;
    case MEAU_PARA_0_1:para_huandao();break;
    case MEAU_PARA_0_2:para_huandao();break;
    case MEAU_PARA_0_3:para_huandao();break;
    case MEAU_PARA_0_4:para_huandao();break;    
    case MEAU_GOGOGO:gogogo_choose();break;
    case MEAU_OURTEAM:print_ourteam();break;
    case MEAU_DEBUG_0:debug_camera();break;
    case MEAU_DEBUG_1:debug_adc();break;
    case MEAU_DEBUG_2:debug_wireless();break;
    case MEAU_DEBUG_3:debug_servo();break;
    case MEAU_DEBUG_4:debug_motor();break;
    case MEAU_DEBUG_5:debug_huandao();break;
    case MEAU_DEBUG_6:debug_chasu();break;
    case MEAU_DEBUG_7:debug_20602();break;
    case MEAU_DEBUG_8:debug_apriltag();break;
    case MEAU_DEBUG_9:debug_number();break;
    case MEAU_GOGOGO_0:gogogo_choose();break;
    case MEAU_GOGOGO_1:gogogo_choose();break;
    case MEAU_GOGOGO_2:gogogo_choose();break;
    case MEAU_GOGOGO_3:gogogo_choose();break;
    case MEAU_GOGOGO_4:gogogo_choose();break;
    case MEAU_GOGOGO_5:gogogo_choose();break;
    case MEAU_GOGOGO_6:gogogo_run();break;
    case MEAU_NON:break;//可删去
    }
}


void show_arrow(uint8 arrow)
{
    if(arrow!=MEAU_NON)
    {
        for(int i=0;i<10;i++)
            (i==arrow)?lcd_showstr(14*8,i,"<-"):lcd_showstr(14*8,i,"  ");
    }

}
void debug_camera()
{
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy						
            binary_img();			
            Search();
            Y_Change();
            lcd_displayimage032_zoom(img[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 128, 50);
            lcd_showstr(0,3,"RoundStatus:");
            lcd_showuint8(0,4,Round_Status);
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
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=0;
        }    
        if(sw1_status==1)
            aicar_camera_error();
        else
            servo_duty=3850;
    }
    key4_flag=0;
    servo_duty=3850;
}


void debug_adc()
{
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();
        aicar_adc_printf();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=1;
        }    
        if(sw1_status==1)
            aicar_adc_error();
        else
            servo_duty=3850;
    }
    key4_flag=0;
    servo_duty=3850;
}


void debug_wireless()
{
    stop_cnt=0;
    break_flag=0;
    aim_speed=30;
    adc_isr_enable=1;
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy						
            binary_img();			
            Search();
          
            //aicar_camera_wireless(USART_8, img[0], MT9V03X_CSI_W, MT9V03X_CSI_H);//山外上位机
            csi_seekfree_sendimg_03x(USART_8,img[0],MT9V03X_CSI_W,MT9V03X_CSI_H);//逐飞上位机
        }      
        aicar_n_chasu();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=2;
        }
    }
    break_flag=0;
    key4_flag=0;
    aim_speed=0;
    adc_isr_enable=0;
    servo_duty=3850;
    
}


void debug_servo()
{
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();
        aicar_servopid_printf();
        if(key1_flag&&sw2_status==1)
        {
            key1_flag=0;
            kp_ad+=0.3;
        }
        else if(key2_flag&&sw2_status==1)
        {
            key2_flag=0;
            kp_ad-=0.3;
        }
        else if(key1_flag&&sw2_status==0)
        {
            key1_flag=0;
            kd_ad+=0.3;
        }
        else if(key2_flag&&sw2_status==0)
        {
            key2_flag=0;
            kd_ad-=0.3;
        }
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=3;
        }    
        if(sw1_status==1)
            aicar_adc_error();
        else
            servo_duty=3850;
    }
    key4_flag=0;
    servo_duty=3850;
}


void debug_motor()
{
    stop_cnt=0;
    break_flag=0;
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_motorpid_printf();
        aicar_pid_wireless();
        if(key1_flag)   
        {
            if(sw1_status)
            {
                key1_flag = 0;//使用按键之后，应该清除标志位
                aim_speed+=30;
            }
            else
            {
                key1_flag = 0;//使用按键之后，应该清除标志位
                kp_l+=1.0;
            }
        }
                
        if(key2_flag)       
        {           
            if(sw1_status)
            {
                key2_flag = 0;//使用按键之后，应该清除标志位
                
                aim_speed-=30;
            }
            else
            {
                key2_flag = 0;//使用按键之后，应该清除标志位
                kp_l-=1.0;
            }           
        }
        
        if(key3_flag)           
        {
            if(sw1_status)
            {
                key3_flag = 0;//使用按键之后，应该清除标志位
                ki_l+=1.0;
            }
            else
            {
                key3_flag = 0;//使用按键之后，应该清除标志位
                ki_l-=1.0;
            }
        }
        
        
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=4;
        }    
        aicar_n_chasu();
    }
    key4_flag=0;
    break_flag=1;
    aim_speed=0;
    servo_duty=3850;   
}


void debug_huandao()
{
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();

        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy	
            binary_img();				   				
            Search();    
        }       
        aicar_huandao();
        aicar_huandao_printf();

        if(key1_flag)
        {
            key1_flag=0;
            zuo_yuanhuan_flag=1;
        }
        if(key2_flag)
        {
            key2_flag=0;
            you_yuanhuan_flag=1;
        }
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=5;
        }    
        if(sw1_status==1)
            aicar_mix_error();
        else
            servo_duty=3850;
    }
    key4_flag=0;
    servo_duty=3850;
}


void debug_chasu()
{
    stop_cnt=0;
    break_flag=0;
    aim_speed=SPEED_SET;
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();//停车用
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
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy	
            binary_img();				   				
            Search();    
        }        
        if(flag_Cross==1)
            aicar_adc_error();
        else
            aicar_mix_error();
       
        aicar_chasu_printf();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=6;
        }
    }
    break_flag=1;
    key4_flag=0;
    aim_speed=0;
    servo_duty=3850;
}

void debug_20602()
{
    turn_sum=0;
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();

        get_icm20602_accdata_spi();
        get_icm20602_gyro_spi();
        turn_sum+=icm_gyro_z;
            
        aicar_20602_printf();        
        if(key1_flag)
        {
            key1_flag=0;
        }
        if(key2_flag)
        {
            key2_flag=0;
        }
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_DEBUG;
            pointer_arrow=7;
        }    
    }
    key4_flag=0;
    servo_duty=3850;
    
}


void debug_apriltag()
{
    find_apriltag();
    lcd_clear(BLACK);
    pointer_page=MEAU_DEBUG;
    pointer_arrow=8;
}

void debug_number()
{
    sancha_stop();
    lcd_clear(BLACK);
    pointer_page=MEAU_DEBUG;
    pointer_arrow=9;
}

void gogogo_camera()
{
    stop_cnt=0;
    break_flag=0;
    aim_speed=SPEED_SET;
    
    if(meau_garage_mode==GARAGE_LEFT)//车库标志位，左1右2
        aicar_left_garage_out();
    else if(meau_garage_mode==GARAGE_RIGHT)
        aicar_right_garage_out();
    
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();//停车用
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
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            cut_image_to_img2();//copy	
            binary_img();				   				
            Search();    
        }        
        aicar_camera_error();
        
        if(sw1_status==1)
            aicar_chasu();
        else
            aicar_n_chasu();
        lcd_showstr(0,3,"aim_speed:");
        lcd_showint16(10*8,3,aim_speed);
        lcd_showstr(0,4,"chasu:");
        lcd_showuint8(10*8,4,sw1_status);
        lcd_showstr(0,5,"RoundStatus:");
        lcd_showuint8(10*10,5,Round_Status);
        lcd_showstr(0,6,"RoundIN:");
        lcd_showuint8(10*10,6,RoundInCount);
        lcd_showstr(0,7,"RoundOUT:");
        lcd_showuint8(10*10,7,RoundOutCount);
        if(sw2_status==1)
        {
            Y_Change();
            lcd_displayimage032_zoom(img[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 128, 50);
        } 
//        lcd_showstr(0,2,"bk_flag:");
//        lcd_showuint8(12*8,2,break_flag);
//        lcd_showstr(0,4,"kp_ad:");
//        lcd_showfloat(12*8,4,kp_ad,3,2);
//        lcd_showstr(0,5,"kd_ad:");
//        lcd_showfloat(12*8,5,kd_ad,3,2);
//        lcd_showstr(0,6,"left:");
//        lcd_showint16(12*8,6,left_motor);
//        lcd_showstr(0,7,"right:");
//        lcd_showint16(12*8,7,right_motor);
        //aicar_chasu_printf();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_GOGOGO;
            pointer_arrow=0;
        }
    }
    break_flag=1;
    key4_flag=0;
    aim_speed=0;
    servo_duty=3850;
}


void gogogo_adc()
{
    stop_cnt=0;
    break_flag=0;
    aim_speed=SPEED_SET;
    
    if(meau_garage_mode==GARAGE_LEFT)//车库标志位，左1右2
        aicar_left_garage_out();
    else if(meau_garage_mode==GARAGE_RIGHT)
        aicar_right_garage_out();
    
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();
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
        aicar_adc_error();
        
        if(sw1_status==1)
            aicar_chasu();
        else
            aicar_n_chasu();
        lcd_showstr(0,3,"aim_speed:");
        lcd_showint16(10*8,3,aim_speed);
        lcd_showstr(0,4,"chasu:");
        lcd_showuint8(10*8,4,sw1_status);
        lcd_showstr(0,5,"RoundStatus:");
        lcd_showuint8(10*10,5,Round_Status);
        lcd_showstr(0,6,"RoundIN:");
        lcd_showuint8(10*10,6,RoundInCount);
        lcd_showstr(0,7,"RoundOUT:");
        lcd_showuint8(10*10,7,RoundOutCount);
        if(sw2_status==1)
        {
            Y_Change();
            lcd_displayimage032_zoom(img[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 128, 50);
        } 
//        lcd_showstr(0,2,"bk_flag:");
//        lcd_showuint8(12*8,2,break_flag);
//        lcd_showstr(0,4,"kp_ad:");
//        lcd_showfloat(12*8,4,kp_ad,3,2);
//        lcd_showstr(0,5,"kd_ad:");
//        lcd_showfloat(12*8,5,kd_ad,3,2);
//        lcd_showstr(0,6,"left:");
//        lcd_showint16(12*8,6,left_motor);
//        lcd_showstr(0,7,"right:");
//        lcd_showint16(12*8,7,right_motor);
        //aicar_chasu_printf();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_GOGOGO;
            pointer_arrow=1;
        }
    }
    break_flag=1;
    key4_flag=0;
    aim_speed=0;
    servo_duty=3850;
}


void gogogo_mix()
{
    stop_cnt=0;
    break_flag=0;
    aim_speed=SPEED_SET;
    
    if(meau_garage_mode==GARAGE_LEFT)//车库标志位，左1右2
        aicar_left_garage_out();
    else if(meau_garage_mode==GARAGE_RIGHT)
        aicar_right_garage_out();

    
    while(key4_flag!=1)
    {
        aicar_key_get();//按键检测
        aicar_switch_get();//拨码开关
        aicar_adc_get();//停车用
        if(key1_flag)
        {
            key1_flag=0;
            aim_speed_set+=10;
        }
        else if(key2_flag)
        {
            key2_flag=0;
            aim_speed_set-=10;
        }     
        if(mt9v03x_csi_finish_flag)
        {      
            mt9v03x_csi_finish_flag = 0;
            mv_image_to_img2();//copy	
            binary_img();
            Search();
        }        
        //aicar_huandao();//环岛由摄像头给出
        if(flag_Cross==1)
            aicar_adc_error();
        else
            aicar_mix_error();
        
        if(sw1_status==1)
            aicar_chasu();
        else
            aicar_n_chasu();

        lcd_showstr(0,3,"aim_speed:");
        lcd_showint16(10*8,3,aim_speed);
        lcd_showstr(0,4,"chasu:");
        lcd_showuint8(10*8,4,sw1_status);
        lcd_showstr(0,5,"RoundStatus:");
        lcd_showuint8(10*10,5,Round_Status);
        lcd_showstr(0,6,"RoundIN:");
        lcd_showuint8(10*10,6,RoundInCount);
        lcd_showstr(0,7,"RoundOUT:");
        lcd_showuint8(10*10,7,RoundOutCount);
        if(sw2_status==1)
        {
            Y_Change();
            lcd_displayimage032_zoom(img[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 128, 50);
        }                  
        
        if(flag_Y_Road)
        {            
            lcd_showstr(0,8,"Y_Road");
        }
        else if(Round_Status)
        {
            lcd_showstr(0,8,"Round");
        }
        else if(flag_Garage_L || flag_Garage_R)
        {
            lcd_showstr(0,8,"Garage");
        }
        else if(flag_Cross)
        {
            lcd_showstr(0,8,"Cross");
        }
        else if(flag_T_Road)
        {
            lcd_showstr(0,8,"T_Road");
        }
        else
        {
            lcd_showstr(0,8,"Normal");
        }

//        lcd_showstr(0,2,"bk_flag:");
//        lcd_showuint8(12*8,2,break_flag);
//        lcd_showstr(0,4,"kp_ad:");
//        lcd_showfloat(12*8,4,kp_ad,3,2);
//        lcd_showstr(0,5,"kd_ad:");
//        lcd_showfloat(12*8,5,kd_ad,3,2);
//        lcd_showstr(0,6,"left:");
//        lcd_showint16(12*8,6,left_motor);
//        lcd_showstr(0,7,"right:");
//        lcd_showint16(12*8,7,right_motor);
        //aicar_chasu_printf();
        if(key4_flag)
        {
            lcd_clear(BLACK);
            pointer_page=MEAU_GOGOGO;
            pointer_arrow=0;
        }
    }
    break_flag=1;
    key4_flag=0;
    aim_speed=0;
    servo_duty=3850;
}


void para_huandao()
{
    lcd_showstr(0,0,"in:");
    lcd_showstr(0,1,"out:");
    lcd_showstr(0,2,"in_delay:");
    lcd_showstr(0,3,"out_delay:");
    lcd_showstr(0,4,"turn:");
    lcd_showstr(0,5,"init");    
    lcd_showuint8(10*8,0,hd_in);
    lcd_showuint8(10*8,1,hd_out);
    lcd_showuint8(10*8,2,hd_in_delay);
    lcd_showuint8(10*8,3,hd_out_delay);
    lcd_showuint8(10*8,4,hd_turn);    
}

void gogogo_choose()
{
    lcd_showstr(0,0,"run_mode:");
    lcd_showstr(0,1,"garage:");
    lcd_showstr(0,2,"magic");
    lcd_showstr(0,3,"apriltag");
    lcd_showstr(0,4,"servo");
    lcd_showstr(0,5,"ani_or_fru");  
    lcd_showstr(0,6,"GOGOGO!");
    switch(meau_run_mode)
    {
    case GOGOGO_ADC: 
        lcd_showstr(10*8,0,"ADC  ");break;
    case GOGOGO_MIX:
        lcd_showstr(10*8,0,"MIX  ");break;
    case GOGOGO_CAM:
        lcd_showstr(10*8,0,"CAM  ");break;
    }
    switch(meau_garage_mode)
    {
    case GARAGE_LEFT: 
        lcd_showstr(10*8,1,"L  ");break;
    case GARAGE_RIGHT:
        lcd_showstr(10*8,1,"R  ");break;
    case GARAGE_NON:
        lcd_showstr(10*8,1,"NON");break;
    }
    if(magic_mode==1)
    {
        lcd_showstr(10*8,2,"ON");
    }
    else if(magic_mode==0)
    {           
        lcd_showstr(10*8,2,"OFF");
    }
}

void gogogo_run()
{
    switch(meau_run_mode)
    {
    case GOGOGO_ADC: 
        gogogo_adc();break;
    case GOGOGO_MIX:
        gogogo_mix();break;
    case GOGOGO_CAM:
        gogogo_camera();break;
    }
}

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
#include "aicar_meau_main.h"
#include "aicar_element.h"
#include "aicar_flash.h"
#include "SEEKFREE_18TFT.h"

vuint8 meau_run_mode=GOGOGO_MIX;//选择跑车模式
vuint8 meau_garage_mode=GARAGE_LEFT;//选择出库模式
uint8 pointer_page=0;
uint8 pointer_arrow=0;


void aicar_meau_init()
{
    lcd_clear(BLACK);
    pointer_page=MEAU_MAIN;
    pointer_arrow=0;
}


void aicar_meau_scan()
{
//    aicar_switch_get();//拨码开关预留位，可放标志位
//    if(sw1_status==0&&sw2_status==0)
//    {
//            
//        
//    }    
//    else if(sw1_status==0&&sw2_status==1)   
//    {
//            
//
//    }        
//    else if(sw1_status==1&&sw2_status==0)
//    {
//            
//    }
//    else if(sw1_status==1&&sw2_status==1)
//    {
//            
//    }
    aicar_key_get();//按键控制菜单
    if(key1_flag)
    {
        key1_flag=0;      
        up_status();
    }
    else if(key2_flag)
    {
        key2_flag=0;
        down_status();
    }
    else if(key3_flag)
    {
        key3_flag=0;
        go_status();
    }
    else if(key4_flag)
    {
        key4_flag=0;
        back_status();
    }        
}
    

void up_status()
{
    switch(pointer_page)
    {
    case MEAU_MAIN:
        pointer_arrow-=1;
        if(pointer_arrow>3)
            pointer_arrow=3;break;
    case MEAU_DEBUG:
        pointer_arrow-=1;
        if(pointer_arrow>9)
            pointer_arrow=9;break;   
    case MEAU_PARA:
        pointer_arrow-=1;
        if(pointer_arrow>3)
            pointer_arrow=3;break; 
    case MEAU_PARA_0:
        pointer_arrow-=1;
        if(pointer_arrow>5)
            pointer_arrow=5;break; 
    case MEAU_PARA_0_0:
        hd_in+=1;break;       
    case MEAU_PARA_0_1:
        hd_out+=1;break;    
    case MEAU_PARA_0_2:
        hd_in_delay+=1;break; 
    case MEAU_PARA_0_3:
        hd_out_delay+=1;break; 
    case MEAU_PARA_0_4:
        hd_turn+=1;break;    
    case MEAU_GOGOGO:
        pointer_arrow-=1;
        if(pointer_arrow>6)
            pointer_arrow=6;break; 
    case MEAU_GOGOGO_0:
        if(meau_run_mode==GOGOGO_MIX)
        {
            lcd_showstr(10*8,0,"   ");
            meau_run_mode=GOGOGO_ADC;
        }
        else if(meau_run_mode==GOGOGO_CAM)
        {
            lcd_showstr(10*8,0,"   ");
            meau_run_mode=GOGOGO_MIX;
        }
        else if(meau_run_mode==GOGOGO_ADC)
        {
            lcd_showstr(10*8,0,"   ");
            meau_run_mode=GOGOGO_CAM;
        }break; 
    case MEAU_GOGOGO_1:
        if(meau_garage_mode==GARAGE_LEFT)
        {
            lcd_showstr(10*8,1,"     ");
            meau_garage_mode=GARAGE_RIGHT;
        }
        else if(meau_garage_mode==GARAGE_RIGHT)
        {
            lcd_showstr(10*8,1,"     ");
            meau_garage_mode=GARAGE_NON;
        }
        else if(meau_garage_mode==GARAGE_NON)
        {
            lcd_showstr(10*8,1,"     ");
            meau_garage_mode=GARAGE_LEFT;
        }break; 
    case MEAU_GOGOGO_2:
        if(magic_mode==0)
        {
            lcd_showstr(10*8,2,"     ");
            magic_mode=1;
        }
        else if(magic_mode==1)
        {
            lcd_showstr(10*8,2,"     ");
            magic_mode=0;
        }break;
    case MEAU_OURTEAM:
        pointer_arrow-=1;
        if(pointer_arrow>2)
            pointer_arrow=2;break; 
        
    }
}
void down_status()
{
    switch(pointer_page)
    {
    case MEAU_MAIN:
        pointer_arrow+=1;
        if(pointer_arrow>3)
            pointer_arrow=0;break;
    case MEAU_DEBUG:
        pointer_arrow+=1;
        if(pointer_arrow>9)
            pointer_arrow=0;break;       
    case MEAU_PARA:
        pointer_arrow+=1;
        if(pointer_arrow>3)
            pointer_arrow=0;break; 
    case MEAU_PARA_0:
        pointer_arrow+=1;
        if(pointer_arrow>5)
            pointer_arrow=0;break; 
    case MEAU_PARA_0_0:
        hd_in-=1;break;       
    case MEAU_PARA_0_1:
        hd_out-=1;break;    
    case MEAU_PARA_0_2:
        hd_in_delay-=1;break; 
    case MEAU_PARA_0_3:
        hd_out_delay-=1;break;  
    case MEAU_PARA_0_4:
        hd_turn-=1;break;          
    case MEAU_GOGOGO:
        pointer_arrow+=1;
        if(pointer_arrow>6)
            pointer_arrow=0;break; 
    case MEAU_GOGOGO_0:
        if(meau_run_mode==GOGOGO_MIX)
        {
            meau_run_mode=GOGOGO_CAM;
            lcd_showstr(10*8,0,"   ");            
        }
        else if(meau_run_mode==GOGOGO_CAM)
        {
            meau_run_mode=GOGOGO_ADC;
            lcd_showstr(10*8,0,"   ");            
        }
        else if(meau_run_mode==GOGOGO_ADC)
        {
            meau_run_mode=GOGOGO_MIX;
            lcd_showstr(10*8,0,"     ");                      
        }break; 
    case MEAU_GOGOGO_1:
        if(meau_garage_mode==GARAGE_LEFT)
        {
            meau_garage_mode=GARAGE_NON;
            lcd_showstr(10*8,1,"     ");           
        }
        else if(meau_garage_mode==GARAGE_RIGHT)
        {
            meau_garage_mode=GARAGE_LEFT;
            lcd_showstr(10*8,1,"     ");           
        }
        else if(meau_garage_mode==GARAGE_NON)
        {
            meau_garage_mode=GARAGE_RIGHT;
            lcd_showstr(10*8,1,"     ");
            
        }break; 
    case MEAU_GOGOGO_2:
        if(magic_mode==0)
        {
            magic_mode=1;
            lcd_showstr(10*8,2,"     ");           
            lcd_showstr(10*8,2,"ON");
        }
        else if(magic_mode==1)
        {
            magic_mode=0;            
            lcd_showstr(10*8,2,"     ");
            lcd_showstr(10*8,2,"OFF");
        }break;
    case MEAU_OURTEAM:
        pointer_arrow+=1;
        if(pointer_arrow>2)
            pointer_arrow=0;break;   
    }
}
void go_status()
{
    switch(pointer_page)
    {
    case MEAU_MAIN:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_DEBUG;break;
        case 1:pointer_page=MEAU_PARA;break;
        case 2:pointer_page=MEAU_GOGOGO;break;
        case 3:pointer_page=MEAU_OURTEAM;break;
        }       
        pointer_arrow=0;
        lcd_clear(BLACK);break;
    case MEAU_DEBUG:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_DEBUG_0;break;
        case 1:pointer_page=MEAU_DEBUG_1;break;
        case 2:pointer_page=MEAU_DEBUG_2;break;
        case 3:pointer_page=MEAU_DEBUG_3;break;
        case 4:pointer_page=MEAU_DEBUG_4;break;
        case 5:pointer_page=MEAU_DEBUG_5;break;//huandao
        case 6:pointer_page=MEAU_DEBUG_6;break;//chasu
        case 7:pointer_page=MEAU_DEBUG_7;break;//20602   
        case 8:pointer_page=MEAU_DEBUG_8;break;
        case 9:pointer_page=MEAU_DEBUG_9;break;
        }       
        pointer_arrow=0;
        lcd_clear(BLACK);break;
    case MEAU_PARA:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_PARA_0;break;
        }       
        pointer_arrow=0;
        lcd_clear(BLACK);break;
    case MEAU_PARA_0:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_PARA_0_0;break;
        case 1:pointer_page=MEAU_PARA_0_1;break;
        case 2:pointer_page=MEAU_PARA_0_2;break;
        case 3:pointer_page=MEAU_PARA_0_3;break;
        case 4:pointer_page=MEAU_PARA_0_4;break;        
        case 5:aicar_flash_init();break;
        }       
        pointer_arrow=MEAU_NON;
        lcd_clear(BLACK);break;
    case MEAU_PARA_0_0:aicar_flash_write();break;  
    case MEAU_PARA_0_1:aicar_flash_write();break; 
    case MEAU_PARA_0_2:aicar_flash_write();break; 
    case MEAU_PARA_0_3:aicar_flash_write();break; 
    case MEAU_PARA_0_4:aicar_flash_write();break;    
          
    case MEAU_GOGOGO:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_GOGOGO_0;break;//模式选择
        case 1:pointer_page=MEAU_GOGOGO_1;break;//左右出库
        case 2:pointer_page=MEAU_GOGOGO_2;break;//magic_mode
        case 3:pointer_page=MEAU_GOGOGO_3;break;//magic_mode apriltag
        case 4:pointer_page=MEAU_GOGOGO_4;break;//magic_mode number
        case 5:pointer_page=MEAU_GOGOGO_5;break;//magic_mode animal_fruit
        case 6:pointer_page=MEAU_GOGOGO_6;break;//GOGOGO!
        }       
        pointer_arrow=MEAU_NON;
        lcd_clear(BLACK);break;
    }
}
void back_status()
{
    switch(pointer_page)
    {
    case MEAU_MAIN:
        bb_time=10;
        pointer_arrow=0;break;
    case MEAU_DEBUG:
        pointer_page=MEAU_MAIN;
        pointer_arrow=0;break;
    case MEAU_PARA:
        pointer_page=MEAU_MAIN;
        pointer_arrow=1;break;
    case MEAU_PARA_0:
        pointer_page=MEAU_PARA;
        pointer_arrow=0;break;   
    case MEAU_PARA_0_0:
        pointer_page=MEAU_PARA_0;
        pointer_arrow=0;break;      
    case MEAU_PARA_0_1:
        pointer_page=MEAU_PARA_0;
        pointer_arrow=1;break;     
    case MEAU_PARA_0_2:
        pointer_page=MEAU_PARA_0;
        pointer_arrow=2;break;   
    case MEAU_PARA_0_3:
        pointer_page=MEAU_PARA_0;
        pointer_arrow=3;break;    
    case MEAU_PARA_0_4:
        pointer_page=MEAU_PARA_0;
        pointer_arrow=4;break;         
    case MEAU_GOGOGO:
        pointer_page=MEAU_MAIN;
        pointer_arrow=2;break;
    case MEAU_GOGOGO_0:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=0;break; 
    case MEAU_GOGOGO_1:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=1;break; 
    case MEAU_GOGOGO_2:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=2;break; 
    case MEAU_GOGOGO_3:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=3;break; 
    case MEAU_GOGOGO_4:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=4;break; 
    case MEAU_GOGOGO_5:
        pointer_page=MEAU_GOGOGO;
        pointer_arrow=5;break; 
    case MEAU_OURTEAM:
        pointer_page=MEAU_MAIN;
        pointer_arrow=3;break;        
    }
    lcd_clear(BLACK);
}            
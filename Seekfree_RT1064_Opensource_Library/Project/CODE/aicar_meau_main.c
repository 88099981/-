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
#include "SEEKFREE_18TFT.h"

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
    aicar_switch_get();//拨码开关预留位，可放标志位
    if(sw1_status==0&&sw2_status==0)
    {
            
        
    }    
    else if(sw1_status==0&&sw2_status==1)   
    {
            

    }        
    else if(sw1_status==1&&sw2_status==0)
    {
            
    }
    else if(sw1_status==1&&sw2_status==1)
    {
            
    }
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
        if(pointer_arrow>4)
            pointer_arrow=4;break;   
    case MEAU_PARA:
        pointer_arrow-=1;
        if(pointer_arrow>3)
            pointer_arrow=3;break; 
    case MEAU_GOGOGO:
        pointer_arrow-=1;
        if(pointer_arrow>2)
            pointer_arrow=2;break; 
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
        if(pointer_arrow>4)
            pointer_arrow=0;break;       
    case MEAU_PARA:
        pointer_arrow+=1;
        if(pointer_arrow>3)
            pointer_arrow=0;break; 
    case MEAU_GOGOGO:
        pointer_arrow+=1;
        if(pointer_arrow>2)
            pointer_arrow=0;break; 
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
        }       
        pointer_arrow=0;
        lcd_clear(BLACK);break;
    case MEAU_GOGOGO:       
        switch (pointer_arrow)
        {
        case 0:pointer_page=MEAU_GOGOGO_0;break;
        case 1:pointer_page=MEAU_GOGOGO_1;break;
        case 2:pointer_page=MEAU_GOGOGO_2;break;
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
    case MEAU_GOGOGO:
        pointer_page=MEAU_MAIN;
        pointer_arrow=2;break;
    case MEAU_OURTEAM:
        pointer_page=MEAU_MAIN;
        pointer_arrow=3;break;        
    }
    lcd_clear(BLACK);
}            
/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用： flash                       00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_flash.h"
#include "aicar_init.h"
#include "aicar_element.h"
#include "fsl_cache.h"
#include "zf_flash.h"

uint32 write_buf[FLASH_SAVE_NUM];
uint32 read_buf[FLASH_SAVE_NUM];
uint8 status;

void aicar_flash_read()
{
    flash_read_page(EXAMPLE_FLASH_SECTOR, EXAMPLE_FLASH_SECTOR_PAGE, read_buf, FLASH_SAVE_NUM);
    //分别读取对应数据
    hd_in = read_buf[0];
    hd_out = read_buf[1];
    hd_in_delay = read_buf[2];
    hd_out_delay = read_buf[3];      //读取浮点数时，应该使用宏函数将uint32类型转换为float
    hd_turn = read_buf[4]; 
}


void aicar_flash_write()
{
    bb_time=5;
    if(flash_check(EXAMPLE_FLASH_SECTOR,EXAMPLE_FLASH_SECTOR_PAGE))//校验当前 扇区所在页是否有数据，如果有数据则擦除整个扇区
    {
        status = flash_erase_sector(EXAMPLE_FLASH_SECTOR);//擦除扇区，如果扇区已经有数据则必须擦除扇区之后才能再次写入新的数据
        if(status)  while(1);//擦除失败
    }
    
    write_buf[0] = hd_in;
    write_buf[1] = hd_out;
    write_buf[2] = hd_in_delay;
    write_buf[3] = hd_out_delay; 
    write_buf[4] = hd_turn;    
    //存储浮点时，首先取变量地址然后以uint32 *来访问变量获取数据。
    //write_buf[3] = *(uint32 *)&write_data4; 
    //不能使用此类格式write_buf[3] = (uint32)write_data4;这样会导致强制转换为整型，导致小数部分丢失
    //也可以使用库提供的宏定义 读取float类型的二进制数据

    
    status = flash_page_program(EXAMPLE_FLASH_SECTOR, EXAMPLE_FLASH_SECTOR_PAGE, write_buf, FLASH_SAVE_NUM);
    if(status)  while(1);//写入失败

}


void aicar_flash_init()
{
    hd_in=HD_IN;
    hd_out=HD_OUT;
    hd_in_delay=HD_IN_DELAY;
    hd_out_delay=HD_OUT_DELAY;
    hd_turn=HD_TURN;
    aicar_flash_write();
}
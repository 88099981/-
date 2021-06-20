#ifndef _aicar_flash_h
#define _aicar_flash_h

#include"common.h"
//定义flash最后一个扇区，避免与程序冲突
//存储参数最好从最后一个扇区开始使用
#define EXAMPLE_FLASH_SECTOR        (FLASH_SECTOR_NUM-1)

//定义所在扇区的页编号
#define EXAMPLE_FLASH_SECTOR_PAGE   (0)

#define FLASH_SAVE_NUM  4//宏定义需要存储参数的个数    一个页最多为256字节，一个参数需要占4个字节，因此最多这里为256/4=64
extern uint32 write_buf[FLASH_SAVE_NUM];
extern uint32 read_buf[FLASH_SAVE_NUM];

void aicar_flash_read();
void aicar_flash_write();
void aicar_flash_init();

#endif
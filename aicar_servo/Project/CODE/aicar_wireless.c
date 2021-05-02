/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用：wireless                        00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_wireless.h"
#include "zf_uart.h"
#include "Serial_oscilloscope.h"
#include "SEEKFREE_MT9V03X.h"
#include "aicar_init.h"


void aicar_pid_wireless(void)
{
    encoder_str[0]=encoder1;
    encoder_str[1]=encoder2;
    encoder_str[2]=left_motor;
    encoder_str[3]=right_motor;
    encoder_str[4]=aim_speed;
    encoder_str[5]=0;
    encoder_str[6]=0;
    encoder_str[7]=0;
    Data_Send(WIRELESS_UART, encoder_str);
}

void aicar_camera_wireless(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height)
{
    uart_putchar(uartn,0x01);uart_putchar(uartn,0xFE);//发送帧头
    uart_putbuff(uartn, image, width*height);  //发送图像
    uart_putchar(uartn,0xFE);uart_putchar(uartn,0x01);//发送帧尾
}


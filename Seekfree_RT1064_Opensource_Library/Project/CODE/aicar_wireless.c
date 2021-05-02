/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã�wireless                        00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_wireless.h"
#include "zf_uart.h"
#include "Serial_oscilloscope.h"
#include "SEEKFREE_MT9V03X.h"
#include "aicar_init.h"
#include "aicar_motor.h"


void aicar_pid_wireless(void)
{
    encoder_str[0]=encoder1;
    encoder_str[1]=encoder2;
    encoder_str[2]=(uint16)speed_out1/10;
    encoder_str[3]=(uint16)speed_out2/10;
    encoder_str[4]=(uint16)aim_speed/10;
    encoder_str[5]=(uint16)kp_l*100;
    encoder_str[6]=(uint16)ki_l*100;
    encoder_str[7]=0;
    Data_Send(WIRELESS_UART, encoder_str);
}

void aicar_camera_wireless(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height)
{
    uart_putchar(uartn,0x01);uart_putchar(uartn,0xFE);//����֡ͷ
    uart_putbuff(uartn, image, width*height);  //����ͼ��
    uart_putchar(uartn,0xFE);uart_putchar(uartn,0x01);//����֡β
}


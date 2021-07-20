#ifndef _aicar_uart_h
#define _aicar_uart_h

#include"common.h"

#define E_START                 0       //准备状态
#define E_OK                    1       //成功
#define E_FRAME_HEADER_ERROR    2       //帧头错误
#define E_FRAME_RTAIL_ERROR     3       //帧尾错误

#define LINE_LEN                6      //数据长度
extern uint8   temp_buff[LINE_LEN];            //主机用于接收数据的BUFF
extern vuint8  uart_flag;                      //接收数据标志位


extern vuint8 show_flag;                        //数据显示标志位
extern vuint8 test_getdata;
extern uint8 apriltag_delay;
//函数
void get_slave_data(uint8 data);
void data_analysis(uint8 *line);
void aicar_uart_init();
void aicar_uart_callback();
#endif

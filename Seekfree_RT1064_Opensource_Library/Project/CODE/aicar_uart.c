/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               东秦ai视觉一队 编写                 00**
**00                  作者：刘坤龙                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            函数作用： servo                        00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_uart.h"
#include "aicar_adc.h"
#include "aicar_init.h"
#include "aicar_element.h"
#include "zf_uart.h"

uint8   temp_buff[LINE_LEN]={0};            //主机用于接收数据的BUFF
vuint8  uart_flag=0;                      //接收数据标志位

uint8 example_rx_buffer;
//uint8 example_rx_buffer[LINE_LEN];

lpuart_transfer_t   example_receivexfer;
lpuart_handle_t     example_g_lpuartHandle;

//vuint8 show_flag=0;                        //数据显示标志位
vuint8 test_getdata=0;
uint8 uart_num = 0;
//函数
//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取从机数据
//  @param      data            串口数据
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void get_slave_data(uint8 data)
{
    temp_buff[uart_num++] = data;

    if(1 == uart_num)
    {
        //接收到的第一个字符不为0xD8，帧头错误
        if(0xD8 != temp_buff[0])
        {
            uart_num = 0;
            uart_flag = E_FRAME_HEADER_ERROR;
        }
    }

    if(LINE_LEN == uart_num)
    {
        //接收到最后一个字节为0xEE
        if(0xEE == temp_buff[LINE_LEN - 1])
        {
            uart_flag = E_OK;
        }
        else    //接收到最后一个字节不是0xEE，帧尾错误
        {
            uart_flag = E_FRAME_RTAIL_ERROR;
        }
        uart_num = 0;
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      根据协议对从机发送过来的数据，进行数据解析
//  @param      *line                           串口接收到的数据BUFF
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void data_analysis(uint8 *line)//01 云台左 02云台右 03舵机左 04舵机右 05动物 06水果
{
    if(line[1] == 0x01)    holder_l_turn();
    else if(line[1] == 0x02)    holder_r_turn();
    if(line[2] == 0x03){
        if(magic_mode){//自选
            switch (magic_data[0]){
            case SERVO_LEFT: servo_l_turn();
            case SERVO_RIGHT: servo_r_turn();
            }
        }
        else servo_l_turn();
    }
    else if(line[2] == 0x04){
        if(magic_mode){//自选
            switch (magic_data[0]){
            case SERVO_LEFT: servo_l_turn();
            case SERVO_RIGHT: servo_r_turn();
            }
        }
        else servo_r_turn();
    }       
}


void aicar_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    if(kStatus_LPUART_RxIdle == status)
    {
        //数据已经被写入到了 之前设置的BUFF中
        //本例程使用的BUFF为 example_rx_buffer
        get_slave_data(example_rx_buffer);//读取帧头帧尾
    }
    
    handle->rxDataSize = example_receivexfer.dataSize;  //还原缓冲区长度
    handle->rxData = example_receivexfer.data;          //还原缓冲区地址
}


void aicar_uart_init()
{
    uart_init (USART_1, 115200,UART1_TX_B12,UART1_RX_B13 );	
    NVIC_SetPriority(LPUART1_IRQn,15);         //设置串口中断优先级 范围0-15 越小优先级越高
    uart_rx_irq(USART_1,1);
    
    //配置串口接收的缓冲区及缓冲区长度
    example_receivexfer.dataSize = 1;
    example_receivexfer.data = &example_rx_buffer;
    
    //设置中断函数及其参数
    uart_set_handle(USART_1, &example_g_lpuartHandle, aicar_uart_callback, NULL, 0, example_receivexfer.data, 1);

}
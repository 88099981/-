/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã� servo                        00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_uart.h"
#include "aicar_adc.h"
#include "aicar_init.h"
#include "aicar_element.h"
#include "zf_uart.h"

uint8   temp_buff[LINE_LEN]={0};            //�������ڽ������ݵ�BUFF
vuint8  uart_flag=0;                      //�������ݱ�־λ

uint8 example_rx_buffer;
//uint8 example_rx_buffer[LINE_LEN];

lpuart_transfer_t   example_receivexfer;
lpuart_handle_t     example_g_lpuartHandle;

//vuint8 show_flag=0;                        //������ʾ��־λ
vuint8 test_getdata=0;
uint8 uart_num = 0;
//����
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ�ӻ�����
//  @param      data            ��������
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void get_slave_data(uint8 data)
{
    temp_buff[uart_num++] = data;

    if(1 == uart_num)
    {
        //���յ��ĵ�һ���ַ���Ϊ0xD8��֡ͷ����
        if(0xD8 != temp_buff[0])
        {
            uart_num = 0;
            uart_flag = E_FRAME_HEADER_ERROR;
        }
    }

    if(LINE_LEN == uart_num)
    {
        //���յ����һ���ֽ�Ϊ0xEE
        if(0xEE == temp_buff[LINE_LEN - 1])
        {
            uart_flag = E_OK;
        }
        else    //���յ����һ���ֽڲ���0xEE��֡β����
        {
            uart_flag = E_FRAME_RTAIL_ERROR;
        }
        uart_num = 0;
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����Э��Դӻ����͹��������ݣ��������ݽ���
//  @param      *line                           ���ڽ��յ�������BUFF
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void data_analysis(uint8 *line)//01 ��̨�� 02��̨�� 03����� 04����� 05���� 06ˮ��
{
    if(line[1] == 0x01)    holder_l_turn();
    else if(line[1] == 0x02)    holder_r_turn();
    if(line[2] == 0x03){
        if(magic_mode){//��ѡ
            switch (magic_data[0]){
            case SERVO_LEFT: servo_l_turn();
            case SERVO_RIGHT: servo_r_turn();
            }
        }
        else servo_l_turn();
    }
    else if(line[2] == 0x04){
        if(magic_mode){//��ѡ
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
        //�����Ѿ���д�뵽�� ֮ǰ���õ�BUFF��
        //������ʹ�õ�BUFFΪ example_rx_buffer
        get_slave_data(example_rx_buffer);//��ȡ֡ͷ֡β
    }
    
    handle->rxDataSize = example_receivexfer.dataSize;  //��ԭ����������
    handle->rxData = example_receivexfer.data;          //��ԭ��������ַ
}


void aicar_uart_init()
{
    uart_init (USART_1, 115200,UART1_TX_B12,UART1_RX_B13 );	
    NVIC_SetPriority(LPUART1_IRQn,15);         //���ô����ж����ȼ� ��Χ0-15 ԽС���ȼ�Խ��
    uart_rx_irq(USART_1,1);
    
    //���ô��ڽ��յĻ�����������������
    example_receivexfer.dataSize = 1;
    example_receivexfer.data = &example_rx_buffer;
    
    //�����жϺ����������
    uart_set_handle(USART_1, &example_g_lpuartHandle, aicar_uart_callback, NULL, 0, example_receivexfer.data, 1);

}
/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã�  go                          00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_gogogo.h"
#include "headfile.h"

void aicar_gogogo()
{
    DisableGlobalIRQ();
    board_init();           //��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    systick_delay_ms(1000);

    aicar_init();
    //aicar_switch_get();�ܳ�ǰѡ״̬
    pit_init();                     //��ʼ��pit����
    pit_interrupt_ms(PIT_CH0,10);  //��ʼ��pitͨ��0 ����
    NVIC_SetPriority(PIT_IRQn,5);  //�����ж����ȼ� ��Χ0-15 ԽС���ȼ�Խ�� ��·PIT����һ��PIT�жϺ���
    servo_duty=3850;
    
    //��ʼ���������ж��Ƕ�̬�ģ������һ����ʼ������������Ƶ�init�ļ�
    kp_l=KP_motor_left;
    ki_l=KI_motor_left;
    kp_r=KP_motor_right;
    ki_r=KI_motor_right;
    kp_ad=KP_ad_str;
    kd_ad=KD_ad_str;
    //a_cam=A_cam;//������ϵ������������
    kp_cam=KP_cam;
    kd_cam=KD_cam;
    ostu_thres=203;//��ֵ

    EnableGlobalIRQ(0); //���ж������
    while(1)
    {
        aicar_meau();
    }
}
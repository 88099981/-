/**********************************************************
**0000000000000000000000000000000000000000000000000000000**
**00                                                   00**
**00               ����ai�Ӿ�һ�� ��д                 00**
**00                  ���ߣ�������                     00**
**00                                                   00**
**0000000000000000000000000000000000000000000000000000000**
**00            �������ã� chasu                       00**
**0000000000000000000000000000000000000000000000000000000**
**********************************************************/
#include "aicar_chasu.h"
#include "aicar_adc.h"
#include "aicar_init.h"
void aicar_chasu()//���Ͷ���й�
{
    chasu_b=servo_duty*0.1175-452.375;
    chasu_b=chasu_b*0.01745329;
    chasu_b=sin(chasu_b);
    if(chasu_b<EPSOLON&&chasu_b>-EPSOLON)
    {
        chasu_k=1;
        left_motor=aim_speed;
        right_motor=aim_speed;
    }
    else if(chasu_b>=EPSOLON)
    {
        chasu_k=1+(10.5/(20.0/chasu_b-9.25));
        left_motor=aim_speed;
        right_motor=(int16)(aim_speed*chasu_k);
    }
    else if(chasu_b<=-EPSOLON)
    {
        chasu_b=-chasu_b;
        chasu_k=1+(10.5/(20.0/chasu_b-9.25));
        left_motor=(int16)(aim_speed*chasu_k);
        right_motor=aim_speed;           
    }
}

void aicar_n_chasu()
{
    left_motor=aim_speed;
    right_motor=aim_speed;
}
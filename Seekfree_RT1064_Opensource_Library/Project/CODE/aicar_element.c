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
#include "aicar_element.h"
#include "aicar_init.h"
#include "aicar_adc.h"
#include "fd_GetMid.h"
vuint8 ruhuan=0,chuhuan=0;
vuint8 chuhuan_delay=0;
vuint8 ruhuan_delay=0;
vuint8 ruhuan_wait=0;

void aicar_yuanhuan()//�������ж�
{
    if(chuhuan_delay>0)
    {
        chuhuan_delay--;
    }
    if(ruhuan_delay>0)
    {
        ruhuan_delay--;
    }
//    if(ruhuan_wait>0)//�������㣬��ֹ���Ժ����Ӱ��
//    {
//        ruhuan_wait--;
//        if(ruhuan_wait<=0)
//        {
//            zuo_yuanhuan_flag=0;
//            you_yuanhuan_flag=0;
//        }
//    }
    
    if(ad_value_all>400&&chuhuan_delay<=0)
    {
        if(zuo_yuanhuan_flag==0&&you_yuanhuan_flag==0)
        {
            ruhuan++;
            if(flag_Round_ARM_L&&ruhuan>=10)//zuo
            {                                
                 zuo_yuanhuan_flag=1;
                 ruhuan=0;
                 bb_time=20;              
            }
            else if(flag_Round_ARM_R&&ruhuan>=10)//you
            {
                 you_yuanhuan_flag=1;
                 ruhuan=0;
                 bb_time=20;
            }
        }
        else
        {
            chuhuan++;
            if(zuo_yuanhuan_flag==1&&chuhuan>=10)
            {
                zuo_yuanhuan_flag=0;
                //camera_down=100;
                chuhuan=0;
                bb_time=60;
                chuhuan_delay=200;//��ֹ�����뻷
            }
            else if(you_yuanhuan_flag==1&&chuhuan>=10)
            {
                you_yuanhuan_flag=0;
                //camera_down=100;
                chuhuan=0;
                bb_time=60;
                chuhuan_delay=200;
            }

        }
    }
    else
    {
        ruhuan=0;
        chuhuan=0;
    }

//    if(x_move_time>0)
//    {
//        x_move_time--;
//        if(camera_down==1&&x_move_time==0)
//        {
//            camera_down==0;
//        }
//        //aim_x=100;
//
//    }

//    sqrt_left=sqrt(ad_left);
//    sqrt_right=sqrt(ad_right);

}
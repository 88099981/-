#ifndef _FD_GETMID_H__
#define _FD_GETMID_H__

#include "headfile.h"

#define Black (0x00)
#define White (0xff)
#define Gray  (0x80)
#define IMG_X 188 //ͼ������С
#define IMG_Y 50 //ͼ�������С
#define Y_LIMIT 4   //���������߽�
#define X_LIMIT 4   //���������߽�
#define EDGE_MAX IMG_X-2*X_LIMIT //�߽������С
#define MID_RESOLUTION 20   //���߽����� ԽСԽ�ӽ�ԭʼ����

#define WIDTH_BASE 80 //��ײ�������ȱ궨ֵ
#define WIDTH_K 0.98    //��խ����

//struct declear
typedef struct
{
    uint8 Lx;
    uint8 Rx;
}EDGE;

//function
void init(void);    //���ֱ�����ʼ������
void Y_Change(void);    //�ĳ�һ���ֱ������ϵ��ʽ��Ҫ��Ȼ����ˬ
void Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r);
void Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y);
void Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y);
uint8 Hor_Search_Base(uint8 MidStart,uint8 y);
uint8 Hor_Search(uint8 MidStart,uint8 Y);    //ˮƽɨ�� MidStartΪ����ɨ����ʼ�е� yΪɨ���Ӧ�ĸ߶�
uint8 Ver_Search(uint8 Midstart);   //�Ӹ����е㴹ֱɨ�赽����
uint8 Uni_Ver_Search(uint8 MidStart);
void Search(void);  //��Ե���Һ���
uint8 Judge(void);   //Ԫ���ж�
uint8 Width_Cali(uint8);    //������ȼ���
void Damn_Lose_Edge_all(void);  //˫�ඪ��
void If_Lose_Edge(void);    //���߲�ȫ
void Set_Mid(void); //��������
void Print_Mid(void);   //�滭����
void Simple_Err_Check(void);    //�������߼��
void CAM_Error(void);

//data
extern uint8 img[IMG_Y][IMG_X];
extern uint8 EdgeNum;
extern int16 mid[];
#endif

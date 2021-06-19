#ifndef _FD_GETMID_H__
#define _FD_GETMID_H__

#include "headfile.h"

#define IMG_DEBUG   //ͼ����Ա�־�����ڼ���ĳЩ������������ʾ

#define Black (0x00)
#define White (0xff)
#define Gray  (0x80)
#define IMG_X 188 //ͼ������С
#define IMG_Y 50 //ͼ�������С
#define Y_LIMIT 4   //���������߽�
#define X_LIMIT 4   //���������߽�
#define EDGE_MAX IMG_X-2*X_LIMIT //�߽������С
#define MID_RESOLUTION 20   //���߽����� ԽСԽ�ӽ�ԭʼ����
#define CUR_RESOLUTION 10   //�ǶȽ����� ��������FA��FB��A_y-B_y�Ĳ�ֵ�������Ƚϴֲڣ���С��������У���������

#define ROUND_ARM_COUNT_TIMES 200

#define WIDTH_BASE 80 //��ײ�������ȱ궨ֵ
#define WIDTH_K 0.98    //��խ����

//struct declear
typedef struct
{
    int16 Lx;
    int16 Rx;
}EDGE;

typedef struct
{
    float L;
    float R;
}ANGLE;

//function
void init(void);    //���ֱ�����ʼ������
void Y_Change(void);    //�ĳ�һ���ֱ������ϵ��ʽ��Ҫ��Ȼ����ˬ
float Cp_sqrt(float number);    //���ٿ�����
float Get_Angle(uint8 p1_y,uint8 p2_y,uint8 Target_y,uint8 l_or_r);
void Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r);
void Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y);
void Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y);
uint8 Hor_Search_Base(uint8 MidStart,uint8 y);
uint8 Hor_Search(uint8 MidStart,uint8 Y);    //ˮƽɨ�� MidStartΪ����ɨ����ʼ�е� yΪɨ���Ӧ�ĸ߶�
uint8 Ver_Search(uint8 Midstart);   //�Ӹ����е㴹ֱɨ�赽����
uint8 Uni_Ver_Search(uint8 MidStart);
uint8 Feature_Verify(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 *feature);    //�����ȽϺ����������������ͼ���Ӧλ�ý��бȽϣ��������ƶ�(0~100)
uint8 Judge(void);   //״̬��
uint8 Width_Cali(uint8);    //������ȼ���
void Damn_Lose_Edge_all(void);  //˫�ඪ��
void If_Lose_Edge(void);    //���߲�ȫ
void If_Straight(void); //ֱ���ж�
void Set_Mid(void); //��������
void Print_Mid(void);   //�滭����
void Search(void);  //��Ե���Һ���
void Simple_Err_Check(void);    //�������߼��
void CAM_Error(void);

//data
extern uint8 img[IMG_Y][IMG_X];
extern uint8 EdgeNum;
extern int16 mid[];

//flag
extern uint8 flag_T_Road;
extern uint8 flag_Cross;
extern uint8 flag_Round_ARM_L;
extern uint8 flag_Round_ARM_R; 
extern uint8 flag_Round_ARM_L_B;
extern uint8 flag_Round_ARM_R_B;
extern uint8 flag_Is_This_Round;
extern uint8 flag_Round_in_L;
extern uint8 flag_Round_in_R;
extern uint8 flag_Normal_Lose_L;
extern uint8 flag_Normal_Lose_R;
extern uint8 flag_AprilTag_ARM;
extern uint8 flag_AprilTag;

//----����/������־----//
extern uint8 flag_Indct_Help_Me;


#endif

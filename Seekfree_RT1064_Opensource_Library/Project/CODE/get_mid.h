#ifndef __GET_MID_H__
#define __GET_MID_H__

//#include "IMG_include.h"
#include "camera.h"

uint8 Search_main(void);					//������

extern uint8 flag_type;				//��·���ࣨ1=ֱ����2=��ת�䣬3=ʮ��·��4=Y��·��5=������0=����ǧ��Ҫ��ʼ������
extern uint8 flag_type_Number;		//��ֵΪflag_type_L-1��Ϊʮλ��flag_type_R-1��Ϊ��λ
extern uint8 flag_type_curve;		//��ת�����ࣨ1=L��2=R��0=ֱ����
extern uint8 flag_type_cross;		//ʮ�ֽ׶Σ�1=δ����ֱ����2=ʮ�֣�3=δ��ǰֱ��,4=����ģʽ����������֪,5=����ģʽ����������֪��0=ֱ����
extern uint8 flag_type_Y;			//Y���ࣨ0=ֱ��,ûд��
extern uint8 flag_type_round;		//�����׶�&���ࣨ0=ֱ��,1�󻷵���ʼ��2�һ�����ʼ��3�󻷵��ڶ��׶Σ�4�һ����ڶ��׶Σ�

extern uint8 L_edge_Num;			//����ظ�������Ҫ��ʼ��Ϊ0����
extern uint8 R_edge_Num;			//�ұ��ظ�������Ҫ��ʼ��Ϊ0����
extern uint8 Mid_X[Num_mid];		//�е������(X����)��1��ʼ��0����������
extern uint8 Mid_Y[Num_mid];		//�е������(Y����)��1��ʼ��0����������
extern uint8 Mid_Num;				//ʵ����������

void Y_change(void);

#endif

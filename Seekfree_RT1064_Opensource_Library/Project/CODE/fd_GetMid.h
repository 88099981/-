#ifndef _FD_GETMID_H__
#define _FD_GETMID_H__

#include "headfile.h"

#define Black (0x00)
#define White (0xff)
#define Gray  (0x80)
#define IMG_X 188 //图像横轴大小
#define IMG_Y 50 //图像纵轴大小
#define Y_LIMIT 4   //纵轴搜索边界
#define X_LIMIT 4   //横轴搜索边界
#define EDGE_MAX IMG_X-2*X_LIMIT //边界数组大小
#define MID_RESOLUTION 20   //中线解析度 越小越接近原始数据

#define WIDTH_BASE 80 //最底层赛道宽度标定值
#define WIDTH_K 0.98    //变窄比例

//struct declear
typedef struct
{
    uint8 Lx;
    uint8 Rx;
}EDGE;

//function
void init(void);    //各种变量初始化函数
void Y_Change(void);    //改成一般的直角坐标系形式，要不然贼不爽
void Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r);
void Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y);
void Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y);
uint8 Hor_Search_Base(uint8 MidStart,uint8 y);
uint8 Hor_Search(uint8 MidStart,uint8 Y);    //水平扫描 MidStart为给定扫描起始中点 y为扫描对应的高度
uint8 Ver_Search(uint8 Midstart);   //从给定中点垂直扫描到黑区
uint8 Uni_Ver_Search(uint8 MidStart);
void Search(void);  //边缘查找函数
uint8 Judge(void);   //元素判断
uint8 Width_Cali(uint8);    //赛道宽度计算
void Damn_Lose_Edge_all(void);  //双侧丢边
void If_Lose_Edge(void);    //丢边补全
void Set_Mid(void); //设置中线
void Print_Mid(void);   //绘画中线
void Simple_Err_Check(void);    //简易中线检错
void CAM_Error(void);

//data
extern uint8 img[IMG_Y][IMG_X];
extern uint8 EdgeNum;
extern int16 mid[];
#endif

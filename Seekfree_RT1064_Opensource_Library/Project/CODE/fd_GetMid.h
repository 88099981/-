#ifndef _FD_GETMID_H__
#define _FD_GETMID_H__

#include "headfile.h"

#define IMG_DEBUG   //图像调试标志，用于激活某些调试特征的显示

#define Black (0x00)
#define White (0xff)
#define Gray  (0x80)
#define Mark_Lane (0x60)
#define Mark_AprilTag (0xa0)
#define IMG_X 188 //图像横轴大小
#define IMG_Y 50 //图像纵轴大小
#define PIX_IMG_X IMG_X/3
#define PIX_IMG_Y IMG_Y/3
#define Y_LIMIT 4   //纵轴搜索边界
#define X_LIMIT 4   //横轴搜索边界
#define EDGE_MAX IMG_Y//边界数组大小
#define MID_RESOLUTION 20   //中线解析度 越小越接近原始数据
#define CUR_RESOLUTION 10   //角度解析度 就是向量FA、FB中A_y-B_y的差值，过大会比较粗糙，过小会过于敏感，容易误判

#define ROUND_ARM_COUNT_TIMES 20

#define WIDTH_BASE 80 //最底层赛道宽度标定值
#define WIDTH_K 0.98    //变窄比例

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

typedef struct 
{
    int8 offset_x;
    int8 offset_y;
}DIRECTION;

typedef struct
{
    uint16 lane;
    uint16 AprilTag;
}SUMINCD;

//function
void init(void);    //各种变量初始化函数
void Y_Change(void);    //改成一般的直角坐标系形式，要不然贼不爽
float Cp_sqrt(float number);    //快速开方求导
float Get_Angle(uint8 p1_y,uint8 p2_y,uint8 Target_y,uint8 l_or_r);
uint8 Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r);
uint8 Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y);
uint8 Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y);
uint8 Hor_Search_Base(uint8 MidStart,uint8 y);
uint8 Hor_Search(uint8 MidStart,uint8 Y);    //水平扫描 MidStart为给定扫描起始中点 y为扫描对应的高度
uint8 zoomin(float weight,uint8 PIX_DeBUG); //缩放效果
uint8 Ver_Search(uint8 Midstart);   //从给定中点垂直扫描到黑区
uint8 Uni_Ver_Search(uint8 MidStart);
uint8 Feature_Verify(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 *feature);    //特征比较函数，将特征数组和图像对应位置进行比较，返回相似度(0~100)]
uint8 Feature_Verify_Color(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 color,float expect_rate);
uint8 Feature_Verify_Box(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 thickness,uint8 white_or_black);
void Copy_pix_img(void);
uint8 SetSeed_Lane(uint8 MidStart,uint8 EdgeNum);
uint8 SetSeed_AprilTag(uint8 T_y,uint8 T_color);
void FloodSeedFill(uint8 x,uint8 y,uint8 Old_Color,uint8 New_Color);
uint8 If_Garage(void);  //车库查找  优先级大于环岛、十字/T字
uint8 Judge(void);   //状态判断
uint8 Width_Cali(uint8);    //赛道宽度计算
void Damn_Lose_Edge_all(void);  //双侧丢边
uint8 If_Lose_Edge(void);    //丢边补全
void If_Straight(void); //直道判断
void Set_Mid(void); //设置中线
void Print_Mid(void);   //绘画中线
void Search(void);  //主函数

//data
extern uint8 temp1;
extern uint8 temp2;
extern uint8 img[IMG_Y][IMG_X];
extern uint8 pix_img[PIX_IMG_Y][PIX_IMG_X];
extern uint8 copy_pix_img[PIX_IMG_Y][PIX_IMG_X];
extern uint8 EdgeNum;
extern int16 mid[];
extern uint8 Round_Status;
extern uint16 Round_ad_limit;
extern uint16 RoundInCount;
extern uint8 RoundOutCount;
extern uint8 YRoadInCount;
extern uint16 SumInCD;
extern SUMINCD sumincd;

//flag
extern uint8 flag_Straight;
extern uint8 flag_T_Road;
extern uint8 flag_Y_Road;
extern uint16 flag_Y_Road_IN;
extern uint8 Y_Road_Status;
extern uint8 flag_Cross;
extern uint8 flag_Normal_Lose_L;
extern uint8 flag_Normal_Lose_R;
extern uint8 flag_Garage_L;
extern uint8 flag_Garage_R;
extern uint8 flag_AprilTag;

//----故障/辅助标志----//
extern uint8 flag_Indct_Help_Me;


#endif

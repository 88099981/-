#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdio.h>
#include "include.h"

//typedef unsigned char       uint8;  /*  8 bits */
//typedef unsigned short int  uint16; /* 16 bits */
//typedef unsigned long int   uint32; /* 32 bits */
//typedef unsigned long long  uint64; /* 64 bits */

//typedef char                int8;   /*  8 bits */
//ypedef short int           int16;  /* 16 bits */
//typedef long  int           int32;  /* 32 bits */
//typedef long  long          int64;  /* 64 bits */

#define White (0xff)				//白色
#define Gray (0x80)					//灰色
#define Black (0x00)				//黑色
#define IMAGE_H 50				//提取图像的高度 120
#define IMAGE_W 188             //提取图像的宽度
#define IMG_H 50					//预处理后图像的高度 
#define IMG_W 188					//预处理后图像的宽度
#define IMG_MID	94					//IMG_W/2
#define Num_edge 200				//预计边沿存在的个数
#define Num_mid 200					//预计中点存在的个数
#define Num_up_search_skip 2		//从第N行向两边搜索
#define Num_lim_search_UP 2			//搜索上边界极限，必须大于等于2
#define Num_lim_search_LR 2			//搜索左右边界极限，必须大于等于2
#define Num_search_UP_Turn1 30		//图像上方区域划分为三份（是否直道，左右边相同）
#define Num_search_UP_Turn2 158		//图像上方区域划分为三份（是否直道，左右边相反）
#define Num_search_LR_Turn1 10		//图像左右边界划分为三份（下中分界点）
#define Num_search_LR_Turn2 25		//图像左右边界划分为三份（中上分界点）
#define Search_NUM_1_X (edge_X-1)	//九宫格扫描坐标
#define Search_NUM_2_X edge_X		//九宫格扫描坐标
#define Search_NUM_3_X (edge_X+1)	//九宫格扫描坐标
#define Search_NUM_4_X (edge_X-1)	//九宫格扫描坐标
#define Search_NUM_5_X (edge_X+1)	//九宫格扫描坐标
#define Search_NUM_6_X (edge_X-1)	//九宫格扫描坐标
#define Search_NUM_7_X edge_X		//九宫格扫描坐标
#define Search_NUM_8_X (edge_X+1)	//九宫格扫描坐标
#define Search_NUM_1_Y (edge_Y+1)	//九宫格扫描坐标
#define Search_NUM_2_Y (edge_Y+1)	//九宫格扫描坐标
#define Search_NUM_3_Y (edge_Y+1)	//九宫格扫描坐标
#define Search_NUM_4_Y edge_Y		//九宫格扫描坐标
#define Search_NUM_5_Y edge_Y		//九宫格扫描坐标
#define Search_NUM_6_Y (edge_Y-1)	//九宫格扫描坐标
#define Search_NUM_7_Y (edge_Y-1)	//九宫格扫描坐标
#define Search_NUM_8_Y (edge_Y-1)	//九宫格扫描坐标
#define NUM_Search_8_special_Mod 12 //九宫格扫描特殊情况个数
#define NUM_curve_Mod 4				//转弯模型个数
#define NUM_curve_special_Mod 9		//特殊弯道模型个数
#define NUM_curve_S_Mod 5			//急转弯模型个数
#define NUM_cross_Mod 9				//十字路模型个数
#define NUM_No_Mod 5				//缺少一边模型个数
#define SP_curve_or_Cross 20		//三行变黑，十字还是弯道，边界阈值
#define Num_Croos_find_turn_point 3 //十字拐点两边为白色的数量（判断依据）
#define Num_Search_to_UP_skip 6		//在向上搜索时第一次可以跳过几个点进行搜索
#define Cross_3_Make_edge 30		//十字第三阶段两边画黑线
#define Search_Start_NUM_ 15			//寻找起始点时特殊情况开始点
#define NUM_block_route 4			//多少个连续线为通路
#define Search_up_NOedge_30_Move 10			//向上搜索时用30没找见进行X移动，最多移动多少
#define Cross_Turn_point_too_LOW 6			//十字拐点太低,Y坐标和边界数量
#define Route_NUM 7							//联通个数
#define angle_is_S 129				//锐角极限
#define angle_is_L 4				//直线极限（不等于，小于）

#define TwoPointIsSame 5			//这个范围内认为两个点是相同的
#define LineJudgeEdgeNumMin 20		//直线判断时要求这个线的数组大于N
#define	WarningNum 20				//警告模式扫描的一个数值，与IMG_W相关
#define SearchToUPMaxSearchNum 6	//向上搜索时判断连续同色的数量的最大值
#define CrossEdgeNumMustGreaterThen 70 //环岛中十字每个边的数量都大于N

//extern uint8 mt9v03x_csi_image[IMAGE_H][IMAGE_W];
extern uint8 img[IMG_H][IMG_W];		//收到的图像
extern uint8 IMG_mid_L;				//图像偏左中点
extern uint8 IMG_mid_R;				//图像偏右中点
#endif

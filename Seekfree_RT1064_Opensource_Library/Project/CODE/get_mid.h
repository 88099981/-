#ifndef __GET_MID_H__
#define __GET_MID_H__

//#include "IMG_include.h"
#include "camera.h"

uint8 Search_main(void);					//主函数

extern uint8 flag_type;				//道路种类（1=直道，2=急转弯，3=十字路，4=Y型路，5=环岛，0=错误）千万不要初始化！！
extern uint8 flag_type_Number;		//数值为flag_type_L-1作为十位，flag_type_R-1作为个位
extern uint8 flag_type_curve;		//急转弯种类（1=L，2=R，0=直道）
extern uint8 flag_type_cross;		//十字阶段（1=未见后直道，2=十字，3=未见前直道,4=特殊模式且左赛道已知,5=特殊模式且右赛道已知，0=直道）
extern uint8 flag_type_Y;			//Y种类（0=直道,没写）
extern uint8 flag_type_round;		//环岛阶段&种类（0=直道,1左环岛初始，2右环岛初始，3左环岛第二阶段，4右环岛第二阶段）

extern uint8 L_edge_Num;			//左边沿个数，需要初始化为0！！
extern uint8 R_edge_Num;			//右边沿个数，需要初始化为0！！
extern uint8 Mid_X[Num_mid];		//中点放数组(X坐标)从1开始，0放特殊坐标
extern uint8 Mid_Y[Num_mid];		//中点放数组(Y坐标)从1开始，0放特殊坐标
extern uint8 Mid_Num;				//实际中线数量

void Y_change(void);

#endif

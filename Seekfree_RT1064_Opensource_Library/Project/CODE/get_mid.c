
#include "get_mid.h"

/*局部变量区*/
uint8 IF_Search_8_Cross_SP = 0;		//search8是否初次扫描选择大的值为下一个
uint8 Temp_Num_Rem = 0;				//记录相同标号的个数（特殊模式使用）
uint8 flag_temp_ = 0;
uint8 Cross_L_IF_2 = 0;
uint8 Cross_R_IF_2 = 0;
uint8 NUM_NOT_While_1 = 0;
uint8 Search_8_REM = 0;
/*全局标志区*/
uint8 IF_Warning_MOD = 0;		//1~5和12为warning1，6~11为2
uint8 flag_Event_type = 0;		//事件类型（0为无事件,1为从中间向左边沿未找到跳变点, 2为从中间向右边沿未找到跳变点，3为确实无左边沿，4为确实无右边沿）
uint8 flag_Search_8_RL;		//当前在扫描左边还是右边，左L=1，右R=2，错误=0
uint8 IF_Search_8_Continue;	//是否停止扫描
uint8 flag_Search_8_again = 0;	//是否停止扫描
uint8 flag_type_L;			//千万不要初始化！！左侧道路情况（1=直道，2=左弯道，3=右弯道，4=从左侧上方出屏幕，5=从左侧中间出屏幕，6=从左侧下方出屏幕，7=从右侧上方出屏幕，8=从右侧中间出屏幕，9=从右侧下方出屏幕，10=空，0=错误）
uint8 flag_type_R;			//千万不要初始化！！右侧道路情况（1=直道，2=左弯道，3=右弯道，4=从左侧上方出屏幕，5=从左侧中间出屏幕，6=从左侧下方出屏幕，7=从右侧上方出屏幕，8=从右侧中间出屏幕，9=从右侧下方出屏幕，10=空，0=错误）
uint8 flag_type = 1;			//道路种类（1=直道，2=急转弯，3=十字路，4=Y型路，5=环岛，0=错误）千万不要初始化！！
uint8 flag_type_curve = 0;		//急转弯种类（1=L，2=R，3=L_S,4=R_S,0=直道）
uint8 flag_type_cross = 0;		//十字阶段（1=未见后直道，2=十字，3=未见前直道,4=特殊模式且左赛道已知,5=特殊模式且右赛道已知，0=直道）
uint8 flag_type_Y = 0;			//Y种类（0=直道,没写）
uint8 flag_type_round = 0;		//环岛阶段&种类（0=直道,1左环岛初始，2右环岛初始，3左环岛第二阶段，4右环岛第二阶段）

/*全局变量区*/
#define IF_Y 0
uint8 flag_type_Number;		//数值为flag_type_L-1作为十位，flag_type_R-1作为个位
uint8 IMG_mid_L;
uint8 IMG_mid_R;
uint8 L_edge_Num;			//左边沿个数，需要初始化为0！！
uint8 R_edge_Num;			//右边沿个数，需要初始化为0！！
uint8 L_edge_X[Num_edge];	//左边沿存放数组(X坐标)从1开始，0放特殊坐标
uint8 L_edge_Y[Num_edge];	//左边沿存放数组(Y坐标)从1开始，0放特殊坐标
uint8 R_edge_X[Num_edge];	//右边沿存放数组(X坐标)从1开始，0放特殊坐标
uint8 R_edge_Y[Num_edge];	//右边沿存放数组(Y坐标)从1开始，0放特殊坐标
uint8 L_edge_angle[Num_edge];	//左边沿存放数组(角度)
uint8 R_edge_angle[Num_edge];	//左边沿存放数组(角度)
uint8 Mid_X[Num_mid];		//中点放数组(X坐标)从1开始，0放特殊坐标
uint8 Mid_Y[Num_mid];		//中点放数组(Y坐标)从1开始，0放特殊坐标
uint8 edge_X;				//边界X坐标
uint8 edge_Y;				//边界Y坐标
int8 Search_8_mid_Num_Will_rem[3];//九宫格中心代表的序号（1=左，2=右）
int8 Search_8_mid_Num_Will = 0;	//九宫格中心代表的序号（当前）需要初始化！！
int8 Search_8_mid_Num_Old;	//九宫格中心代表的序号（上上个）
uint8 Mid_Num;				//实际中线数量
uint8 Search_start_Y_L;		//左边沿起始点Y坐标
uint8 Search_start_Y_R;		//右边沿起始点Y坐标
uint8 Croos_turn_point[9];	//十字转折点坐标(偶数为Y坐标，1左下，3右下，5左上，7右上)
uint8 Croos_turn_point_Num[5];	//十字转折点在边界数组的序号(1左下，2右下，3左上，4右上)

/*数据区*/
uint8 Search_8_Mod_1[256] = { 0,1,2,1,3,0,2,1,4,1,0,2,0,0,3,3,5,0,0,1,3,0,2,1,4,1,0,4,3,0,4,4,6,0,0,0,0,0,0,0,4,1,2,2,0,0,3,3,0,0,0,0,0,0,0,0,4,1,0,5,0,0,5,5,7,0,0,1,0,0,2,1,0,1,0,2,0,0,0,3,0,0,0,0,3,0,2,1,0,0,0,0,0,0,0,0,6,0,7,0,0,0,0,0,4,1,2,2,0,0,0,3,5,0,0,0,3,0,0,0,4,1,0,0,3,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,1,3,0,2,1,8,0,0,0,3,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,7,0,8,0,0,0,0,0,0,1,0,0,0,0,0,0,5,0,2,1,3,0,2,1,4,1,0,0,3,0,0,0,6,0,8,0,0,0,0,0,4,1,2,2,0,0,0,3,5,0,2,0,3,0,2,1,4,1,0,0,3,1,0,0 };
uint8 Search_8_Mod_2[256] = { 0,1,2,2,3,0,3,3,4,4,0,4,0,0,4,4,5,0,0,5,5,0,5,5,5,4,0,5,5,0,5,5,6,0,0,0,0,0,0,0,6,6,6,6,0,0,6,6,0,0,0,0,0,0,0,0,6,6,0,6,0,0,6,6,7,0,0,2,0,0,3,3,0,7,0,7,0,0,0,7,0,0,0,0,7,0,7,7,0,0,0,0,0,0,0,0,7,0,6,0,0,0,0,0,7,7,7,7,0,0,0,7,6,0,0,0,6,0,0,0,5,5,0,0,4,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,8,8,0,8,8,5,0,0,0,8,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,7,0,0,0,0,0,0,8,0,0,0,0,0,0,7,0,7,7,7,0,7,7,5,5,0,0,4,0,0,0,8,0,6,0,0,0,0,0,8,8,8,8,0,0,0,8,6,0,6,0,6,0,6,6,5,5,0,0,4,3,0,0 };
uint8 Search_8_Mod_3[12] = { 5,4,5,5,7,7,7,2,4,4,2,2 };
uint8 Search_8_Mod_3_th_To_No[12] = { 25,28,56,57,67,70,71,98,152,156,194,226 };
uint8 Search_8_special_Mod[NUM_Search_8_special_Mod] = { 25,28,56,57,67,70,71,98,152,156,194,226 };
uint8 flag_type_Number_curve_L[NUM_curve_Mod] = { 54,53,43,10 };
uint8 flag_type_Number_curve_R[NUM_curve_Mod] = { 78,68,67,2 };
uint8 flag_type_Number_curve_L_special[NUM_curve_special_Mod] = { 51,50,41,40,31,30,42,52,32 };
uint8 flag_type_Number_curve_R_special[NUM_curve_special_Mod] = { 28,8,27,7,26,6,17,18,16 };
uint8 flag_type_Number_curve_L_S[NUM_curve_S_Mod] = { 95,94,93,91,90 };
uint8 flag_type_Number_curve_R_S[NUM_curve_S_Mod] = { 9,29,69,79,89 };
uint8 flag_type_Number_cross[NUM_cross_Mod] = { 36,47,58,37,48,46,57,56,38 };
uint8 flag_type_Number_No_L[NUM_No_Mod] = { 96,97,98,92,90 };
uint8 flag_type_Number_No_R[NUM_No_Mod] = { 39,49,59,19,9 };

/********************************************函数定义*******************************************/

int16 Round_self(float );
uint8 A_minus_B_abs(uint8 , uint8 );
float SquareRootFloat(float );
void Init(void);
uint8 th_TO_No(uint8 );
void Y_change(void);
void Put_Data_to_L_edge(uint8 , uint8 );
void Put_Data_to_R_edge(uint8 , uint8 );
void Del_Data_Now_L(void);
void Del_Data_Now_R(void);
void Del_Data_N_L(uint8 );
void Del_Data_N_R(uint8 );
void Del_Data_from_N_L(uint8 );
void Del_Data_from_N_R(uint8 );
void Del_Data_from_A_to_B_L(uint8 , uint8 );
void Del_Data_from_A_to_B_R(uint8 , uint8 );
void Check(uint8 );
uint8 IF_A_B_equal(uint8 , uint8 , uint8 );
uint8 Find_MAX_X(uint8 , uint8 , uint8 );
uint8 Find_MIN_X(uint8 , uint8 , uint8 );
uint8 Find_MAX_Y(uint8 , uint8 , uint8 );
uint8 X_or_Y_dif_MAX(uint8 , uint8 , uint8 );
uint8 IF_Same_Point(uint8 );
void Get_angle_L(uint8 , uint8 );
void Get_angle_R(uint8 , uint8 );
uint8 IF_Straight_L(void);
uint8 IF_Straight_R(void);
uint8 IF_Many_edge_Other_Way_L(void);
uint8 IF_Many_edge_Other_Way_R(void);
void Print_Mid_and_Edge(void);
uint8 Get_Curvature(float , float );
void God_FUCK_me(uint8 );
void Mid_Get(void);
void Search_to_L(uint8 );
void Edge_turn(uint8 );
void Violent_Method(void);
void Warning_MOD1(void);
void Err_check(void);
uint8 Search_8_to_Boole(uint8 , uint8 , uint8 );
uint8 Search_8_to_Num(void);
void Search_8_Rem_XY(void);
void Search_8_XY_to_Will(uint8 , uint8 );
uint8 Search_8(void);
void Judge(void);
uint8 Search_Start_MidIsBlack(void);
uint8 Search_for_L_start_point(uint8 );
uint8 Search_for_R_start_point(uint8 );
uint8 Search_to_UP(uint8 , uint8 );
void Croos_find_turn_point_U_L(uint8 );
void Croos_find_turn_point_U_R(uint8 );
uint8 TWO_Point_Get_edge_L(uint8 , uint8 , uint8 , uint8 , uint8 );
uint8 TWO_Point_Get_edge_R(uint8 , uint8 , uint8 , uint8 , uint8 );
void Croos_Search_up_L2(void);
void Croos_Search_up_R2(void);
uint8 Search_8_L(uint8 , uint8 );
uint8 Search_8_R(uint8 , uint8 );
uint8 Get_K_To_get_UP_edge(uint8 );
uint8 Get_K_To_get_Down_edge(uint8 );
uint8 How_many_White_route(uint8 );
uint8 How_many_Black_route(uint8 );
void Find_R_UP_point(uint8 , uint8 );
void Find_L_UP_point(uint8, uint8 );
void Find_R_UP_point_W(uint8 , uint8 );
void Find_L_UP_point_W(uint8 , uint8 );
void Round_main(void);//11以后出问题
void Y_main(void);
void Y_main_2(void);
void Print_for_debug(void);
uint8 Search_main(void);

int16 Round_self(float NUM)
{
	NUM += 0.5;
	return (int16)NUM;
}

uint8 A_minus_B_abs(uint8 A, uint8 B)
{
	uint8 Ans;
	if (A > B)
	{
		Ans = A - B;
	}
	else
	{
		Ans = B - A;
	}
	return Ans;
}

float SquareRootFloat(float number)
{
	long i;
	float x, y;
	const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = *(long*)&y;
	i = 0x5f3759df - (i >> 1);  //卡马克
 //  i  = 0x5f375a86 - ( i >> 1 );  //Lomont
	y = *(float*)&i;
	y = y * (f - (x * y * y));
	y = y * (f - (x * y * y));
	return number * y;
}

//初始化
void Init(void)
{
	Temp_Num_Rem = 0;				//记录相同标号的个数（特殊模式使用）
	Cross_L_IF_2 = 0;
	Cross_R_IF_2 = 0;
	Search_8_REM = 0;
	NUM_NOT_While_1 = 0;
	IF_Search_8_Cross_SP = 0;
	flag_Search_8_again = 0;
	flag_type_L = 1;
	flag_type_R = 1;
	flag_temp_ = 0;
	IF_Warning_MOD = 0;
	L_edge_Num = 0;
	R_edge_Num = 0;
	Mid_Num = 0;
	L_edge_X[0] = Num_lim_search_LR + 1;
	L_edge_Y[0] = Num_up_search_skip;
	R_edge_X[0] = IMG_W - Num_lim_search_LR - 1;
	R_edge_Y[0] = Num_up_search_skip;
	for (uint8 i = 0; i < Num_edge; i++)
	{
		L_edge_X[i] = 0;
		L_edge_Y[i] = 0;
		R_edge_X[i] = 0;
		R_edge_Y[i] = 0;
		L_edge_angle[i] = 0;
		R_edge_angle[i] = 0;
		Mid_X[i] = 0;
		Mid_Y[i] = 0;
	}
	if (!IMG_mid_L || !IMG_mid_R || IMG_mid_L >= IMG_mid_R || IMG_mid_R >= IMG_W - 1)
	{
		IMG_mid_L = IMG_MID - 1;
		IMG_mid_R = IMG_MID;
	}
}

uint8 th_TO_No(uint8 TH)
{
	for (uint8 i = 0; i < 12; i++)
	{
		if (Search_8_Mod_3_th_To_No[i] == TH)
		{
			return Search_8_Mod_3[i];
		}
	}
	return 0;
}

//Y坐标变换（Y=0时是图像的底部即距离小车最近的位置）
void Y_change(void)
{
	uint8 temp;
	for (uint8 i = 0; i < IMG_H / 2; i++)
	{
		for (uint8 j = 0; j < IMG_W; j++)
		{
			temp = img[i][j];
			img[i][j] = img[IMG_H - i - 1][IMG_W - j - 1];
			img[IMG_H - i - 1][IMG_W - j - 1] = temp;
		}
	}
}

//将数据存放到左边沿数组
void Put_Data_to_L_edge(uint8 Data_edge_X, uint8 Data_edge_Y)
{
	if (L_edge_Num < Num_edge - 2)
	{
		L_edge_Num++;
		L_edge_X[L_edge_Num] = Data_edge_X;
		L_edge_Y[L_edge_Num] = Data_edge_Y;
	}
}

//将数据存放到右边沿数组
void Put_Data_to_R_edge(uint8 Data_edge_X, uint8 Data_edge_Y)
{
	if (R_edge_Num < Num_edge - 2)
	{
		R_edge_Num++;
		R_edge_X[R_edge_Num] = Data_edge_X;
		R_edge_Y[R_edge_Num] = Data_edge_Y;
	}
}

//删除上一个边沿数据（左边沿）
void Del_Data_Now_L(void)
{
	if (L_edge_Num)
	{
		L_edge_X[L_edge_Num] = 0;
		L_edge_Y[L_edge_Num] = 0;
		L_edge_Num--;
	}
}

//删除上一个边沿数据（右边沿）
void Del_Data_Now_R(void)
{
	if (R_edge_Num)
	{
		R_edge_X[R_edge_Num] = 0;
		R_edge_Y[R_edge_Num] = 0;
		R_edge_Num--;
	}
}

//删除某一个边沿数据（左边沿）
void Del_Data_N_L(uint8 N)
{
	if (N <= L_edge_Num)
	{
		for (uint8 i = N; i < L_edge_Num; i++)
		{
			L_edge_X[i] = L_edge_X[i + 1];
			L_edge_Y[i] = L_edge_Y[i + 1];
		}
		Del_Data_Now_L();
	}
}

//删除某一个边沿数据（右边沿）
void Del_Data_N_R(uint8 N)
{
	if (N <= R_edge_Num)
	{
		for (uint8 i = N; i < R_edge_Num; i++)
		{
			R_edge_X[i] = R_edge_X[i + 1];
			R_edge_Y[i] = R_edge_Y[i + 1];
		}
		Del_Data_Now_R();
	}
}

//删除从N到当前的边沿数据(删除的数据包含N)（左边沿）
void Del_Data_from_N_L(uint8 N)
{
	if (N <= L_edge_Num)
	{
		for (uint8 i = N; i <= L_edge_Num; i++)
		{
			L_edge_X[i] = 0;
			L_edge_Y[i] = 0;
		}
		L_edge_Num = N - 1;
	}
}

//删除从N到当前的边沿数据(删除的数据包含N)（右边沿）
void Del_Data_from_N_R(uint8 N)
{
	if (N <= R_edge_Num)
	{
		for (uint8 i = N; i <= R_edge_Num; i++)
		{
			R_edge_X[i] = 0;
			R_edge_Y[i] = 0;
		}
		R_edge_Num = N - 1;
	}
}

//删除从A到B的边沿数据(删除的数据包含A和B)（左边沿）
void Del_Data_from_A_to_B_L(uint8 A, uint8 B)
{
	if (A < B && B <= L_edge_Num && A > 0)
	{
		for (uint8 i = 0; i < L_edge_Num - B; i++)
		{
			L_edge_X[A + i] = L_edge_X[B + i + 1];
			L_edge_Y[A + i] = L_edge_Y[B + i + 1];
		}
		for (uint8 i = 0; i <= B - A; i++)
		{
			L_edge_X[L_edge_Num - i] = 0;
			L_edge_Y[L_edge_Num - i] = 0;
		}
		L_edge_Num = L_edge_Num - B + A - 1;
	}
}

//删除从A到B的边沿数据(删除的数据包含A和B)（右边沿）
void Del_Data_from_A_to_B_R(uint8 A, uint8 B)
{
	if (A < B && B <= R_edge_Num && A > 0)
	{
		for (uint8 i = 0; i < R_edge_Num - B; i++)
		{
			R_edge_X[A + i] = R_edge_X[B + i + 1];
			R_edge_Y[A + i] = R_edge_Y[B + i + 1];
		}
		for (uint8 i = 0; i <= B - A; i++)
		{
			R_edge_X[R_edge_Num - i] = 0;
			R_edge_Y[R_edge_Num - i] = 0;
		}
		R_edge_Num = R_edge_Num - B + A - 1;
	}
}

//检测不合理的边界数值
void Check(uint8 L_or_R)
{
	uint8 Err_Num = 250;
	if (L_or_R)
	{
		Err_Num = L_edge_Num;
		for (uint8 i = 1; i <= L_edge_Num; i++)
		{
			if (L_edge_X[i] >= IMG_W || L_edge_Y[i] >= IMG_H)
			{
				Err_Num = i;
				break;
			}
		}
		if (Err_Num != 250)
		{
			Del_Data_from_N_L(Err_Num);
		}
	}
	else
	{
		for (uint8 i = 1; i <= R_edge_Num; i++)
		{
			if (R_edge_X[i] >= IMG_W || R_edge_Y[i] >= IMG_H)
			{
				Err_Num = i;
				break;
			}
		}
		if (Err_Num != 250)
		{
			Del_Data_from_N_R(Err_Num);
		}
	}
}

//检查两个边沿数据是否相等（左边沿=1，右边沿=0）
uint8 IF_A_B_equal(uint8 L_or_R, uint8 A, uint8 B)
{
	uint8 ANS = 0;
	if (L_or_R)
	{
		if (A_minus_B_abs(L_edge_Y[A], L_edge_Y[B]) <= 1)
		{
			ANS = A_minus_B_abs(L_edge_X[A], L_edge_X[B]);
			if (ANS < TwoPointIsSame)
			{
				ANS = 1;
			}
			else
			{
				ANS = 0;
			}
		}
	}
	else
	{
		if (A_minus_B_abs(R_edge_Y[A], R_edge_Y[B]) <= 1)
		{
			ANS = A_minus_B_abs(R_edge_X[A], R_edge_X[B]);
			if (ANS < TwoPointIsSame)
			{
				ANS = 1;
			}
			else
			{
				ANS = 0;
			}
		}
	}
	return ANS;
}

//寻找边界数组中的最大值输入1为左边界，0为右边界
uint8 Find_MAX_X(uint8 L_or_R, uint8 Start_NUM, uint8 End_NUM)
{
	uint8 NUM_MAX = 0;
	uint8 MAX = 0;
	if (L_or_R)
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (L_edge_X[i] >= MAX)
			{
				NUM_MAX = i;
				MAX = L_edge_X[i];
			}
		}
	}
	else
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (R_edge_X[i] >= MAX)
			{
				NUM_MAX = i;
				MAX = R_edge_X[i];
			}
		}
	}
	return NUM_MAX;
}

//寻找边界数组中的最小值输入1为左边界，0为右边界
uint8 Find_MIN_X(uint8 L_or_R, uint8 Start_NUM, uint8 End_NUM)
{
	uint8 NUM_MIN = 0;
	uint8 MIN = 255;
	if (L_or_R)
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (L_edge_X[i] <= MIN)
			{
				NUM_MIN = i;
				MIN = L_edge_X[i];
			}
		}
	}
	else
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (R_edge_X[i] <= MIN)
			{
				NUM_MIN = i;
				MIN = R_edge_X[i];
			}
		}
	}
	return NUM_MIN;
}

//寻找边界数组中的最大值输入1为左边界，0为右边界
uint8 Find_MAX_Y(uint8 L_or_R, uint8 Start_NUM, uint8 End_NUM)
{
	uint8 NUM_MAX = 0;
	uint8 MAX = 0;
	if (L_or_R)
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (L_edge_Y[i] > MAX)
			{
				NUM_MAX = i;
				MAX = L_edge_Y[i];
			}
		}
	}
	else
	{
		for (uint8 i = Start_NUM; i <= End_NUM; i++)
		{
			if (R_edge_Y[i] > MAX)
			{
				NUM_MAX = i;
				MAX = R_edge_Y[i];
			}
		}
	}
	return NUM_MAX;
}

//判断▲X和▲Y哪个大，0=X，1=Y,1=L,0=R
uint8 X_or_Y_dif_MAX(uint8 L_or_R, uint8 Point_A, uint8 Point_B)
{
	uint8 X;
	uint8 Y;
	if (L_or_R)
	{
		X = A_minus_B_abs(L_edge_X[Point_A], L_edge_X[Point_B]);
		Y = A_minus_B_abs(L_edge_Y[Point_A], L_edge_Y[Point_B]);
	}
	else
	{
		X = A_minus_B_abs(R_edge_X[Point_A], R_edge_X[Point_B]);
		Y = A_minus_B_abs(R_edge_Y[Point_A], R_edge_Y[Point_B]);
	}
	if (X > Y)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//判断尖点是否是一个范围中的一点,1=L,0=R
uint8 IF_Same_Point(uint8 L_or_R)
{
	uint8 Y_NUM = Find_MAX_Y(L_or_R, 1, R_edge_Num);
	uint8 X_NUM;
	uint8 D_Y,D_X;
	if (L_or_R)
	{
		X_NUM = Find_MAX_X(1, 1, R_edge_Num);
		if (X_NUM == Y_NUM)
		{
			return 1;
		}
		else
		{
			D_X = A_minus_B_abs(L_edge_X[X_NUM], L_edge_X[Y_NUM]);
			D_Y = A_minus_B_abs(L_edge_Y[X_NUM], L_edge_Y[Y_NUM]);
		}
	}
	else
	{
		X_NUM = Find_MIN_X(0, 1, R_edge_Num);
		if (X_NUM == Y_NUM)
		{
			return 1;
		}
		else
		{
			D_X = A_minus_B_abs(R_edge_X[X_NUM], R_edge_X[Y_NUM]);
			D_Y = A_minus_B_abs(R_edge_Y[X_NUM], R_edge_Y[Y_NUM]);
		}
	}
	D_X += D_Y;
	if (D_X < 10)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//计算角度（左）
void Get_angle_L(uint8 Start_point, uint8 End_point)
{
	float FenZi;
	float Ans;
	int16 X2 = L_edge_X[Start_point];
	int16 X3 = L_edge_X[End_point];
	int16 Y2 = L_edge_Y[Start_point];
	int16 Y3 = L_edge_Y[End_point];
	int16 X1, Y1;
	if (Start_point < End_point)
	{
		L_edge_angle[Start_point] = 0;
		L_edge_angle[End_point] = 0;
		for (uint8 i = Start_point + 1; i < End_point; i++)
		{
			X1 = L_edge_X[i];
			Y1 = L_edge_Y[i];
			FenZi = (X2 - X1) * (X3 - X1) + (Y2 - Y1) * (Y3 - Y1);
			Ans = SquareRootFloat((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
			FenZi = FenZi / Ans;
			Ans = SquareRootFloat((X3 - X1) * (X3 - X1) + (Y3 - Y1) * (Y3 - Y1));
			FenZi = FenZi / Ans;
			FenZi = FenZi * 128 + 128;
			L_edge_angle[i] = FenZi;
		}
	}
}

//计算角度（右）
void Get_angle_R(uint8 Start_point, uint8 End_point)
{
	float FenZi;
	float Ans;
	int16 X2 = R_edge_X[Start_point];
	int16 X3 = R_edge_X[End_point];
	int16 Y2 = R_edge_Y[Start_point];
	int16 Y3 = R_edge_Y[End_point];
	int16 X1, Y1;
	if (Start_point < End_point)
	{
		R_edge_angle[Start_point] = 0;
		R_edge_angle[End_point] = 0;
		for (uint8 i = Start_point + 1; i < End_point; i++)
		{
			X1 = R_edge_X[i];
			Y1 = R_edge_Y[i];
			FenZi = (X2 - X1) * (X3 - X1) + (Y2 - Y1) * (Y3 - Y1);
			Ans = SquareRootFloat((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
			FenZi = FenZi / Ans;
			Ans = SquareRootFloat((X3 - X1) * (X3 - X1) + (Y3 - Y1) * (Y3 - Y1));
			FenZi = FenZi / Ans;
			FenZi = FenZi * 128 + 128;
			R_edge_angle[i] = FenZi;
		}
	}
}

//直线判断（左）
uint8 IF_Straight_L(void)
{
	uint8 Num = 0;
	if (L_edge_Num > LineJudgeEdgeNumMin)
	{
		for (uint8 i = LineJudgeEdgeNumMin/2; i < L_edge_Num - LineJudgeEdgeNumMin / 2; i++)
		{
			if (L_edge_angle[i] > angle_is_L)
			{
				return 0;
			}
			if (!L_edge_angle[i])
			{
				Num++;
			}
		}
	}
	if (Num <= L_edge_Num / 2)
	{
		return 0;
	}
	return 1;
}

//直线判断（右）
uint8 IF_Straight_R(void)
{
	uint8 Num = 0;
	if (R_edge_Num > LineJudgeEdgeNumMin)
	{
		for (uint8 i = LineJudgeEdgeNumMin / 2; i < R_edge_Num - LineJudgeEdgeNumMin / 2; i++)
		{
			if (R_edge_angle[i] > angle_is_L)
			{
				return 0;
			}
			if (!R_edge_angle[i])
			{
				Num++;
			}
		}
	}
	if (Num <= R_edge_Num / 2)
	{
		return 0;
	}
	return 1;
}

//边界太靠近另一侧（左）
uint8 IF_Many_edge_Other_Way_L(void)
{
	uint8 Num = 0;
	for (uint8 i = 1; i <= L_edge_Num; i++)
	{
		if (L_edge_X[i] > IMG_MID)
		{
			Num++;
		}
	}
	if (Num > L_edge_Num / 2)
	{
		return 1;
	}
	return 0;
}

//边界太靠近另一侧（右）
uint8 IF_Many_edge_Other_Way_R(void)
{
	uint8 Num = 0;
	for (uint8 i = 1; i <= R_edge_Num; i++)
	{
		if (R_edge_X[i] < IMG_MID)
		{
			Num++;
		}
	}
	if (Num > R_edge_Num / 2)
	{
		return 1;
	}
	return 0;
}

//绘画中线和边界
void Print_Mid_and_Edge(void)
{
	for (uint8 i = 1; i <= Mid_Num; i++)
	{
		img[Mid_Y[i]][Mid_X[i]] = Gray;
	}
	for (uint8 i = 1; i <= L_edge_Num; i++)
	{
		img[L_edge_Y[i]][L_edge_X[i]] = Gray;
	}
	for (uint8 i = 1; i <= R_edge_Num; i++)
	{
		img[R_edge_Y[i]][R_edge_X[i]] = Gray;
	}
}

//曲率计算,中线计算使用
uint8 Get_Curvature(float N, float Mid_Curvature_MAX)
{
	uint8 number;
	number = Round_self(N / Mid_Curvature_MAX);
	if (number <= 0)
	{
		number = 1;
	}
	if (number > L_edge_Num || number > R_edge_Num)
	{
		if (L_edge_Num > R_edge_Num)
		{
			number = R_edge_Num;
		}
		else
		{
			number = L_edge_Num;
		}
	}
	return number;
}

//无奈之举
void God_FUCK_me(uint8 L_or_R)
{
	if (L_or_R)
	{
		Del_Data_from_N_L(1);
		for (uint8 i = 0; i < IMG_H; i++)
		{
			Put_Data_to_L_edge(0, i);
		}
	}
	else
	{
		Del_Data_from_N_R(1);
		for (uint8 i = 0; i < IMG_H; i++)
		{
			Put_Data_to_R_edge(IMG_W - 1, i);
		}
	}
}

//中线计算
void Mid_Get(void)
{
	float Mid_Curvature_MAX;			//左右边沿曲率计算（L/R或R/L，Mid_Curvature_MAX>1）
	float Mid_Curvature;				//左右边沿曲率计算（L/R，=1直道，>1右拐，<1左拐）
	if (R_edge_Num > Num_edge)
	{
		Del_Data_from_N_R(1);
		God_FUCK_me(0);
	}
	if (L_edge_Num > Num_edge)
	{
		Del_Data_from_N_L(1);
		God_FUCK_me(1);
	}
	Mid_Curvature = (float)L_edge_Num / (float)R_edge_Num;
	if (Mid_Curvature > 1)
	{
		Mid_Curvature_MAX = Mid_Curvature;
		Mid_Num = L_edge_Num;
		for (uint8 i = 1; i <= Mid_Num; i++)
		{
			Mid_X[i] = (L_edge_X[i] + R_edge_X[Get_Curvature(i, Mid_Curvature_MAX)]) / 2;
			Mid_Y[i] = (L_edge_Y[i] + R_edge_Y[Get_Curvature(i, Mid_Curvature_MAX)]) / 2;
		}
	}
	else
	{
		Mid_Curvature_MAX = 1 / Mid_Curvature;
		Mid_Num = R_edge_Num;
		for (uint8 i = 1; i <= Mid_Num; i++)
		{
			Mid_X[i] = (R_edge_X[i] + L_edge_X[Get_Curvature(i, Mid_Curvature_MAX)]) / 2;
			Mid_Y[i] = (R_edge_Y[i] + L_edge_Y[Get_Curvature(i, Mid_Curvature_MAX)]) / 2;
		}
	}
}

//从中间向左边扫描寻找跳变点,flag正常等于1
void Search_to_L(uint8 flag)
{
	flag_Event_type = 1;
	uint8 MID;
	uint8 AIM_ = 1;
	if (flag)
	{
		MID = IMG_mid_L;
		if (img[Search_start_Y_L][IMG_mid_L] == Black || img[Search_start_Y_R][IMG_mid_R] == Black)
		{
			for (uint8 i = 0; i < MID - 1; i++)
			{
				if (MID - i <= Num_lim_search_LR + 1)
				{
					break;
				}
				if (img[Search_start_Y_L][MID - i] == White)
				{
					MID = MID - i - 1;
					break;
				}
			}
		}
		for (uint8 i = 0; i < MID - 1; i++)
		{
			if (MID - i <= Num_lim_search_LR)
			{
				break;
			}
			if (img[Search_start_Y_L][MID - i] == Black)
			{
				edge_X = MID - i;
				edge_Y = Search_start_Y_L;
				Put_Data_to_L_edge(edge_X, edge_Y);
				flag_Event_type = 0;
				break;
			}
		}
		if (flag_Event_type == 1 && Search_start_Y_L >= Num_search_LR_Turn1)
		{
			flag_Event_type = 3;
			flag_type_L = 10;
		}
		if (flag_Event_type == 1)
		{
			Search_start_Y_L++;
			Search_to_L(1);
		}
	}
	else
	{
		for (uint8 i = 1; i <= R_edge_Num; i++)
		{
			if (R_edge_Y[i] == Search_start_Y_L)
			{
				AIM_ = i;
				if (R_edge_Y[i + 1] != Search_start_Y_R)
				{
					break;
				}
			}
		}
		if (R_edge_X[AIM_] < IMG_mid_L)
		{
			if (!R_edge_X[AIM_])
			{
				MID = Search_Start_NUM_;
			}
			else
			{
				for (uint8 i = 1; i <= Search_Start_NUM_; i++)
				{
					MID = R_edge_X[AIM_] - i;
					if (MID < 2 || img[Search_start_Y_L][MID - 2] == Black)
					{
						break;
					}
				}
			}
		}
		else
		{
			MID = Search_Start_NUM_;
		}
		for (uint8 i = 0; i < MID - 1; i++)
		{
			if (MID - i <= Num_lim_search_LR)
			{
				break;
			}
			if (img[Search_start_Y_L][MID - i] == Black)
			{
				edge_X = MID - i;
				edge_Y = Search_start_Y_L;
				Put_Data_to_L_edge(edge_X, edge_Y);
				flag_Event_type = 0;
				break;
			}
		}
		if (flag_Event_type == 1 && Search_start_Y_L >= Num_search_LR_Turn2)
		{
			flag_Event_type = 3;
			flag_type_L = 10;
		}
		if (flag_Event_type == 1)
		{
			Search_start_Y_L++;
			Search_to_L(0);
		}
	}
}

//从中间向右边扫描寻找跳变点,flag正常等于1
void Search_to_R(uint8 flag)
{
	flag_Event_type = 2;
	uint8 MID;
	uint8 AIM_ = 1;
	if (flag)
	{
		MID = IMG_mid_R;
		if (img[Search_start_Y_L][IMG_mid_L] == Black || img[Search_start_Y_R][IMG_mid_R] == Black)
		{
			for (uint8 i = 0; i < MID - 1; i++)
			{
				if (MID + i >= IMG_W - Num_lim_search_LR - 2)
				{
					break;
				}
				if (img[Search_start_Y_R][MID + i] == White)
				{
					MID = MID + i + 1;
					break;
				}
			}
		}
		for (uint8 i = 0; i < IMG_W - MID - 1; i++)
		{
			if (MID + i >= IMG_W - Num_lim_search_LR - 1)
			{
				break;
			}
			if (img[Search_start_Y_R][MID + i] == Black)
			{
				edge_X = MID + i;
				edge_Y = Search_start_Y_R;
				Put_Data_to_R_edge(edge_X, edge_Y);
				flag_Event_type = 0;
				break;
			}
		}
		if (flag_Event_type == 2 && Search_start_Y_R >= Num_search_LR_Turn1)
		{
			flag_Event_type = 4;
			flag_type_R = 10;
		}
		if (flag_Event_type == 2)
		{
			Search_start_Y_R++;
			Search_to_R(1);
		}
	}
	else
	{
		for (uint8 i = 1; i <= L_edge_Num; i++)
		{
			if (L_edge_Y[i] == Search_start_Y_R)
			{
				AIM_ = i;
				if (L_edge_Y[i + 1] != Search_start_Y_R)
				{
					break;
				}
			}
		}
		if (L_edge_X[AIM_] > IMG_mid_R)
		{
			for (uint8 i = 1; i <= Search_Start_NUM_; i++)
			{
				MID = L_edge_X[AIM_] + i;
				if (MID > IMG_W - 3 || img[Search_start_Y_R][MID + 2] == Black)
				{
					break;
				}
			}
		}
		else
		{
			MID = IMG_W - Search_Start_NUM_ - 1;
		}
		for (uint8 i = 0; i < MID - 1; i++)
		{
			if (MID + i >= IMG_W - Num_lim_search_LR - 1)
			{
				break;
			}
			if (img[Search_start_Y_R][MID + i] == Black)
			{
				edge_X = MID + i;
				edge_Y = Search_start_Y_R;
				Put_Data_to_R_edge(edge_X, edge_Y);
				flag_Event_type = 0;
				break;
			}
		}
		if (flag_Event_type == 2 && Search_start_Y_R >= Num_search_LR_Turn2)
		{
			flag_Event_type = 4;
			flag_type_R = 10;
		}
		if (flag_Event_type == 2)
		{
			Search_start_Y_R++;
			Search_to_R(0);
		}
	}
}

//将边界数组倒一下
void Edge_turn(uint8 L_or_R)
{
	uint8 RAM;
	if (L_or_R)
	{
		for (uint8 i = 1; i <= L_edge_Num / 2; i++)
		{
			RAM = L_edge_X[i];
			L_edge_X[i] = L_edge_X[L_edge_Num - i + 1];
			L_edge_X[L_edge_Num - i + 1] = RAM;
			RAM = L_edge_Y[i];
			L_edge_Y[i] = L_edge_Y[L_edge_Num - i + 1];
			L_edge_Y[L_edge_Num - i + 1] = RAM;
		}
	}
	else
	{
		for (uint8 i = 1; i <= R_edge_Num / 2; i++)
		{
			RAM = R_edge_X[i];
			R_edge_X[i] = R_edge_X[R_edge_Num - i + 1];
			R_edge_X[R_edge_Num - i + 1] = RAM;
			RAM = R_edge_Y[i];
			R_edge_Y[i] = R_edge_Y[R_edge_Num - i + 1];
			R_edge_Y[R_edge_Num - i + 1] = RAM;
		}
	}
}

//最暴力的解决方法
void Violent_Method(void)
{
	for (uint8 Y = Num_up_search_skip; Y < IMG_H - Num_lim_search_UP; Y++)
	{
		for (uint8 i = 0; i < IMG_mid_L; i++)
		{
			if (img[Y][IMG_mid_L - i] == Black)
			{
				Put_Data_to_L_edge(IMG_mid_L - i, Y);
				break;
			}
		}
		for (uint8 i = 0; i < IMG_W - IMG_mid_R; i++)
		{
			if (img[Y][IMG_mid_R + i] == Black)
			{
				Put_Data_to_R_edge(IMG_mid_R + i, Y);
				break;
			}
		}
	}
}

//警告模式1,边线无法扫描出来的处理策略
void Warning_MOD1(void)
{
	Init();
	flag_temp_ = 0;
	Search_start_Y_L = Num_up_search_skip;
	Search_start_Y_R = Num_up_search_skip;
	uint8 L_NUM = 0;
	uint8 R_NUM = 0;
	if (img[Search_start_Y_L][IMG_mid_L] == Black && img[Search_start_Y_R][IMG_mid_R] == Black)
	{
		flag_type = 2;
		for (uint8 i = 0; i < IMG_mid_L; i++)
		{
			if (img[Search_start_Y_L][IMG_mid_L - i] == White)
			{
				L_NUM++;
			}
		}
		for (uint8 i = 0; i < IMG_W - IMG_mid_R; i++)
		{
			if (img[Search_start_Y_R][IMG_mid_R + i] == White)
			{
				R_NUM++;
			}
		}
		if (L_NUM < R_NUM)
		{
			flag_Search_8_RL = 1;
			flag_type_curve = 2;
			flag_type_R = 10;
			Put_Data_to_R_edge(IMG_W - 2, 1);
			flag_temp_ = 1;
			for (uint8 Y = Num_up_search_skip; Y < IMG_H - Num_lim_search_UP; Y++)
			{
				if (flag_temp_)
				{
					for (uint8 i = 0; i < IMG_W - IMG_mid_L; i++)
					{
						if (img[Y][IMG_mid_L + i] == White)
						{
							edge_X = IMG_mid_L + i - 1;
							edge_Y = Y;
							Put_Data_to_L_edge(edge_X, edge_Y);
							if (edge_X >= IMG_W - 4)
							{
								flag_temp_ = 0;
							}
							if (L_edge_Num > 2)
							{
								if (L_edge_X[L_edge_Num - 1] > L_edge_X[L_edge_Num])
								{
									Del_Data_Now_L();
								}
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			flag_Search_8_RL = 2;
			flag_type_curve = 1;
			flag_type_L = 10;
			Put_Data_to_L_edge(1, 1);
			flag_temp_ = 1;
			for (uint8 Y = Num_up_search_skip; Y < IMG_H - Num_lim_search_UP; Y++)
			{
				if (flag_temp_)
				{
					for (uint8 i = 0; i < IMG_mid_R; i++)
					{
						if (img[Y][IMG_mid_R - i] == White)
						{
							edge_X = IMG_mid_R - i + 1;
							edge_Y = Y;
							Put_Data_to_R_edge(edge_X, edge_Y);
							if (edge_X <= 3)
							{
								flag_temp_ = 0;
							}
							if (R_edge_Num > 2)
							{
								if (R_edge_X[R_edge_Num - 1] < R_edge_X[R_edge_Num])
								{
									Del_Data_Now_R();
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		Search_start_Y_L = Num_up_search_skip;
		Search_start_Y_R = Num_up_search_skip;
		flag_Search_8_RL = 1;
		Search_to_L(1);
		flag_Search_8_RL = 2;
		Search_to_R(1);
		//左边沿扫描
		flag_temp_ = 1;
		if (flag_type_L != 10)
		{
			while (flag_temp_)
			{
				if (img[L_edge_Y[L_edge_Num] + 1][L_edge_X[L_edge_Num]] == Black)
				{
					for (uint8 i = 0; i < WarningNum; i++)
					{
						flag_temp_ = 0;
						if (L_edge_X[L_edge_Num] + i >= IMG_W - 2)
						{
							break;
						}
						if (L_edge_Y[L_edge_Num] + 1 >= IMG_H - 2)
						{
							break;
						}
						if (L_edge_Num >= Num_edge - 2)
						{
							break;
						}
						if (img[L_edge_Y[L_edge_Num] + 1][L_edge_X[L_edge_Num] + i] == White)
						{
							Put_Data_to_L_edge(L_edge_X[L_edge_Num] + i - 1, L_edge_Y[L_edge_Num] + 1);
							flag_temp_ = 1;
							break;
						}
					}
				}
				else
				{
					for (uint8 i = 0; i < WarningNum; i++)
					{
						flag_temp_ = 0;
						if (L_edge_X[L_edge_Num] - i <= 1)
						{
							break;
						}
						if (img[L_edge_Y[L_edge_Num] + 1][L_edge_X[L_edge_Num] - i] == Black)
						{
							Put_Data_to_L_edge(L_edge_X[L_edge_Num] - i, L_edge_Y[L_edge_Num] + 1);
							flag_temp_ = 1;
							break;
						}
					}
				}
			}
		}
		else
		{
			Put_Data_to_L_edge(1, 1);
		}
		//右边沿扫描
		flag_temp_ = 1;
		if (flag_type_R != 10)
		{
			while (flag_temp_)
			{
				if (img[R_edge_Y[R_edge_Num] + 1][R_edge_X[R_edge_Num]] == Black)
				{
					for (uint8 i = 0; i < WarningNum; i++)
					{
						flag_temp_ = 0;
						if (R_edge_X[R_edge_Num] - i <= 1)
						{
							break;
						}
						if (R_edge_Y[R_edge_Num] + 1 >= IMG_H - 2)
						{
							break;
						}
						if (R_edge_Num >= Num_edge - 2)
						{
							break;
						}
						if (img[R_edge_Y[R_edge_Num] + 1][R_edge_X[R_edge_Num] - i] == White)
						{
							Put_Data_to_R_edge(R_edge_X[R_edge_Num] - i + 1, R_edge_Y[R_edge_Num] + 1);
							flag_temp_ = 1;
							break;
						}
					}

				}
				else
				{
					for (uint8 i = 0; i < WarningNum; i++)
					{
						flag_temp_ = 0;
						if (R_edge_X[R_edge_Num] + i >= IMG_W - 2)
						{
							break;
						}
						if (img[R_edge_Y[R_edge_Num] + 1][R_edge_X[R_edge_Num] + i] == Black)
						{
							Put_Data_to_R_edge(R_edge_X[R_edge_Num] + i, R_edge_Y[R_edge_Num] + 1);
							flag_temp_ = 1;
							break;
						}
					}
				}
			}
		}
		else
		{
			Put_Data_to_R_edge(IMG_W - 2, 1);
		}
	}
	if (flag_type_L == 10 && flag_type_R == 10)
	{
		Init();
		Search_start_Y_L = Num_up_search_skip;
		Search_start_Y_R = Num_up_search_skip;
		Violent_Method();
	}
}

//错误检测*********************************************************************************************************
void Err_check(void)
{
	IF_Warning_MOD = 1;
	//豁免权
	if (flag_type == 3)
	{
		if (flag_type_cross == 1 || flag_type_cross == 2)
		{
			IF_Warning_MOD = 2;
		}
	}
	if (flag_type == 5)
	{
		if (flag_type_round == 1 || flag_type_round == 2)
		{
			IF_Warning_MOD = 2;
		}
	}
	if (IF_Warning_MOD == 1)
	{
		Warning_MOD1();
		//第四步通过两个边沿补全中线
		Mid_Get();
		//绘画中线和边界
		Print_Mid_and_Edge();
		IF_Warning_MOD = 1;
	}
}

//将九宫格中一个格子转换为1/0
uint8 Search_8_to_Boole(uint8 Num_th_Search_8, uint8 Search_8_X, uint8 Search_8_Y)
{
	uint8 temp;
	if (img[Search_8_Y][Search_8_X] == Black)
	{
		temp = (0x01);
	}
	else
	{
		temp = (0x00);
	}
	temp = temp << (Num_th_Search_8 - 1);
	return temp;
}

//将九宫格转换为对应模型序号
uint8 Search_8_to_Num(void)
{
	uint8 Num_th;
	Num_th = 0;
	Num_th += Search_8_to_Boole(1, Search_NUM_1_X, Search_NUM_1_Y);
	Num_th += Search_8_to_Boole(2, Search_NUM_2_X, Search_NUM_2_Y);
	Num_th += Search_8_to_Boole(3, Search_NUM_3_X, Search_NUM_3_Y);
	Num_th += Search_8_to_Boole(4, Search_NUM_4_X, Search_NUM_4_Y);
	Num_th += Search_8_to_Boole(5, Search_NUM_5_X, Search_NUM_5_Y);
	Num_th += Search_8_to_Boole(6, Search_NUM_6_X, Search_NUM_6_Y);
	Num_th += Search_8_to_Boole(7, Search_NUM_7_X, Search_NUM_7_Y);
	Num_th += Search_8_to_Boole(8, Search_NUM_8_X, Search_NUM_8_Y);
	return Num_th;
}

//记录Will的edge_X和edge_Y
void Search_8_Rem_XY(void)
{
	switch (Search_8_mid_Num_Will)
	{
	case 1:
		edge_X = Search_NUM_1_X;
		edge_Y = Search_NUM_1_Y;
		break;
	case 2:
		edge_X = Search_NUM_2_X;
		edge_Y = Search_NUM_2_Y;
		break;
	case 3:
		edge_X = Search_NUM_3_X;
		edge_Y = Search_NUM_3_Y;
		break;
	case 4:
		edge_X = Search_NUM_4_X;
		edge_Y = Search_NUM_4_Y;
		break;
	case 5:
		edge_X = Search_NUM_5_X;
		edge_Y = Search_NUM_5_Y;
		break;
	case 6:
		edge_X = Search_NUM_6_X;
		edge_Y = Search_NUM_6_Y;
		break;
	case 7:
		edge_X = Search_NUM_7_X;
		edge_Y = Search_NUM_7_Y;
		break;
	case 8:
		edge_X = Search_NUM_8_X;
		edge_Y = Search_NUM_8_Y;
		break;
	}
}

//将edge_X和Y转换为will
void Search_8_XY_to_Will(uint8 X, uint8 Y)
{
	if (X == Search_NUM_1_X && Y == Search_NUM_1_Y)
	{
		Search_8_mid_Num_Will = 1;
	}
	if (X == Search_NUM_2_X && Y == Search_NUM_2_Y)
	{
		Search_8_mid_Num_Will = 2;
	}
	if (X == Search_NUM_3_X && Y == Search_NUM_3_Y)
	{
		Search_8_mid_Num_Will = 3;
	}
	if (X == Search_NUM_4_X && Y == Search_NUM_4_Y)
	{
		Search_8_mid_Num_Will = 4;
	}
	if (X == Search_NUM_5_X && Y == Search_NUM_5_Y)
	{
		Search_8_mid_Num_Will = 5;
	}
	if (X == Search_NUM_6_X && Y == Search_NUM_6_Y)
	{
		Search_8_mid_Num_Will = 6;
	}
	if (X == Search_NUM_7_X && Y == Search_NUM_7_Y)
	{
		Search_8_mid_Num_Will = 7;
	}
	if (X == Search_NUM_8_X && Y == Search_NUM_8_Y)
	{
		Search_8_mid_Num_Will = 8;
	}
	edge_X = X;
	edge_Y = Y;
}

//八方扫路法,扫描中心转移+记录
uint8 Search_8(void)
{
	uint8 Mod_1,Mod_2,Mod_3;
	uint8 Search_8_to_Num_th;	//九宫格对应模型序号
	int8 Search_8_mid_Num_Now;	//九宫格中心代表的序号（上一个）
	uint8 flag_Search_8_MOD = 1;	//九宫格模型模式，0为错误模式(模型中没有过来的数值)，1为正常模式，2为特殊模式，3为初次模式&正常模型，4为初次模式&特殊模型，5为特殊模式但上一个是初次模式,6为十字模式左边沿，7为十字模式右边沿
	uint8 flag_Search_8_select = 0;	//选择Search_8_Mod_1还是Search_8_Mod_2或Search_8_Mod_3
	if (R_edge_Num >= Num_edge - 2)
	{
		IF_Search_8_Continue = 0;
		return 0;
	}
	if (L_edge_Num >= Num_edge - 2)
	{
		IF_Search_8_Continue = 0;
		return 0;
	}
	if (edge_X > IMG_W - Num_lim_search_LR || edge_Y > IMG_H - Num_lim_search_UP || !edge_X || !edge_Y)
	{
		IF_Search_8_Continue = 0;
		return 0;
	}
	//Search_8_mid_Num_Now和Search_8_mid_Num_Old递推
	Search_8_mid_Num_Now = 9 - Search_8_mid_Num_Will;
	Search_8_REM = Search_8_mid_Num_Now;

	//对应模型
	Search_8_to_Num_th = Search_8_to_Num();
	Mod_1 = Search_8_Mod_1[Search_8_to_Num_th];
	Mod_2 = Search_8_Mod_2[Search_8_to_Num_th];
	Mod_3 = th_TO_No(Search_8_to_Num_th);
	//检测是一般模式还是特殊模式
	uint8 flag = 1;
	for (uint8 i = 0; i < NUM_Search_8_special_Mod; i++)
	{
		if (Search_8_to_Num_th == Search_8_special_Mod[i])
		{
			flag_Search_8_MOD = 2;
			flag = 0;
			if (Search_8_mid_Num_Will)
			{
				if (flag_Search_8_MOD == 3 || flag_Search_8_MOD == 4)
				{
					flag_Search_8_MOD = 5;
					flag = 0;
				}
			}
			break;
		}
	}
	if (flag)
	{
		flag_Search_8_MOD = 1;
	}

	//检测是否为初次模式和模型是否特殊
	if (!Search_8_mid_Num_Will && flag_Search_8_MOD == 1)
	{
		flag_Search_8_MOD = 3;
	}
	if (!Search_8_mid_Num_Will && flag_Search_8_MOD == 2)
	{
		flag_Search_8_MOD = 4;
	}

	//检测是否为十字模式
	if (!Search_8_mid_Num_Will && flag_Search_8_again)
	{
		if (flag_Search_8_RL == 1)
		{
			flag_Search_8_MOD = 6;
		}
		if (flag_Search_8_RL == 2)
		{
			flag_Search_8_MOD = 7;
		}

	}
	if (IF_Search_8_Cross_SP)
	{
		flag_Search_8_MOD = 8;
	}

	//初始化
	Search_8_mid_Num_Will = 0;

	//对不同模式的处理
	switch (flag_Search_8_MOD)
	{
	case 0://错误
		break;
	case 1://一般模式
		if (Mod_1 == Search_8_mid_Num_Now)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_2 == Search_8_mid_Num_Now)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		break;
	case 2://特殊模式
		Search_8_mid_Num_Old = 9 - Search_8_mid_Num_Now;
		if (!Temp_Num_Rem)//特殊模式阶段1
		{
			flag_Search_8_select = 3;
			Search_8_mid_Num_Will = Mod_3;
			Temp_Num_Rem++;
		}
		if (Temp_Num_Rem == 1)//特殊模式阶段2
		{
			if (Mod_1 != Search_8_mid_Num_Now && Mod_1 != Search_8_mid_Num_Old)
			{
				flag_Search_8_select = 1;
				Search_8_mid_Num_Will = Mod_1;
			}
			if (Mod_2 != Search_8_mid_Num_Now && Mod_2 != Search_8_mid_Num_Old)
			{
				flag_Search_8_select = 2;
				Search_8_mid_Num_Will = Mod_2;
			}
			Temp_Num_Rem = 0;
		}
		break;
	case 3://初次模式&正常模型
		if (Mod_1 > Mod_2)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_1 < Mod_2)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		break;
	case 4://初次模式&特殊模型(暂时和case3一样)
		if (Mod_1 > Mod_2)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_1 < Mod_2)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		break;
	case 5://特殊模式但上一个是初次模式(暂时和case3一样)
		if (Mod_1 > Mod_2)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_1 < Mod_2)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		break;
	case 6://十字模式左
		if (Mod_1 == 2 || Mod_1 == 3 || Mod_1 == 5)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		if (Mod_2 == 2 || Mod_2 == 3 || Mod_2 == 5)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_3 == 2 || Mod_3 == 3 || Mod_3 == 5)
		{
			flag_Search_8_select = 3;
			Search_8_mid_Num_Will = Mod_3;
		}
		if (!Search_8_mid_Num_Will)
		{
			if (Mod_1 == 8 || Mod_2 == 8 || Mod_3 == 8)
			{
				flag_Search_8_select = 1;
				Search_8_mid_Num_Will = 8;
			}
		}
		break;
	case 7://十字模式右
		if (Mod_1 == 1 || Mod_1 == 2 || Mod_1 == 4)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		if (Mod_2 == 1 || Mod_2 == 2 || Mod_2 == 4)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_3 == 1 || Mod_3 == 2 || Mod_3 == 4)
		{
			flag_Search_8_select = 3;
			Search_8_mid_Num_Will = Mod_3;
		}
		if (!Search_8_mid_Num_Will)
		{
			if (Mod_1 == 6 || Mod_2 == 6 || Mod_3 == 6)
			{
				flag_Search_8_select = 1;
				Search_8_mid_Num_Will = 6;
			}
		}
		break;
	case 8://十字模式特殊，初次
		if (Mod_1 < Mod_2)
		{
			flag_Search_8_select = 2;
			Search_8_mid_Num_Will = Mod_2;
		}
		if (Mod_1 > Mod_2)
		{
			flag_Search_8_select = 1;
			Search_8_mid_Num_Will = Mod_1;
		}
		IF_Search_8_Cross_SP = 0;
		break;
	}

	if (!flag_Search_8_select || !Search_8_mid_Num_Will)
	{
		if (flag_Search_8_RL == 1)
		{
			Search_start_Y_L = L_edge_Y[L_edge_Num] + 1;
			Search_to_L(0);
			if (flag_Event_type == 3)
			{
				IF_Search_8_Continue = 0;
			}
			edge_X = L_edge_X[L_edge_Num - 1];
			edge_Y = L_edge_Y[L_edge_Num - 1];
			Search_8_XY_to_Will(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
			if (!Search_8_mid_Num_Will)
			{
				if (NUM_NOT_While_1 > 3)
				{
					Err_check();
					IF_Warning_MOD = 1;
					return 0;
				}
				NUM_NOT_While_1++;
				Search_8();
				NUM_NOT_While_1 = 0;
			}
		}
		if (flag_Search_8_RL == 2)
		{
			Search_start_Y_R = R_edge_Y[R_edge_Num] + 1;
			Search_to_R(0);
			if (flag_Event_type == 4)
			{
				IF_Search_8_Continue = 0;
			}
			edge_X = R_edge_X[R_edge_Num - 1];
			edge_Y = R_edge_Y[R_edge_Num - 1];
			Search_8_XY_to_Will(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
			if (!Search_8_mid_Num_Will)
			{
				if (NUM_NOT_While_1 > 3)
				{
					Err_check();
					IF_Warning_MOD = 1;
					return 0;
				}
				NUM_NOT_While_1++;
				Search_8();
				NUM_NOT_While_1 = 0;
			}
		}
		flag_Search_8_select = 4;
	}

	if (flag_Search_8_select != 4)
	{
		//修改edge的X和Y
		Search_8_Rem_XY();

		//记录边沿
		switch (flag_Search_8_RL)
		{
		case 1:
			Put_Data_to_L_edge(edge_X, edge_Y);
			break;
		case 2:
			Put_Data_to_R_edge(edge_X, edge_Y);
			break;
		}
	}
	if (R_edge_Num >= Num_edge - 2)
	{
		IF_Search_8_Continue = 0;
	}
	if (L_edge_Num >= Num_edge - 2)
	{
		IF_Search_8_Continue = 0;
	}
	return 1;
}

//判断图像的位置,从哪出去
void Judge(void)
{
	if (flag_Search_8_RL == 1)
	{
		if (edge_Y >= IMG_H - Num_lim_search_UP)//从上方出去
		{
			if (img[edge_Y + 1][edge_X] == White)
			{
				img[edge_Y][edge_X] = White;
				Del_Data_Now_L();
				edge_X = L_edge_X[L_edge_Num];
				edge_Y = L_edge_Y[L_edge_Num];
				Search_8_mid_Num_Will = 9 - Search_8_REM;
			}
			else
			{
				IF_Search_8_Continue = 0;
				flag_type_L = 1;
				if (edge_X < Num_search_UP_Turn1)
				{
					flag_type_L = 2;
				}
				if (edge_X > Num_search_UP_Turn2)
				{
					flag_type_L = 3;
				}
			}
		}
		if (edge_X < Num_lim_search_LR)//从左边出去
		{
			IF_Search_8_Continue = 0;
			flag_type_L = 4;
			if (edge_Y < Num_search_LR_Turn2)
			{
				flag_type_L = 5;
			}
			if (edge_Y < Num_search_LR_Turn1)
			{
				flag_type_L = 6;
			}
		}
		if (edge_X >= IMG_W - Num_lim_search_LR)//从右边出去
		{
			IF_Search_8_Continue = 0;
			flag_type_L = 7;
			if (edge_Y < Num_search_LR_Turn2)
			{
				flag_type_L = 8;
			}
			if (edge_Y < Num_search_LR_Turn1)
			{
				flag_type_L = 9;
			}
		}
		if (edge_Y < Num_up_search_skip)
		{
			IF_Search_8_Continue = 0;
			if (edge_X > IMG_mid_R)
			{
				flag_type_L = 9;
			}
			else
			{
				flag_type_L = 6;
			}
		}
	}
	if (flag_Search_8_RL == 2)
	{
		if (edge_Y >= IMG_H - Num_lim_search_UP)//从上方出去
		{
			if (img[edge_Y + 1][edge_X] == White)
			{
				img[edge_Y][edge_X] = White;
				Del_Data_Now_R();
				edge_X = R_edge_X[R_edge_Num];
				edge_Y = R_edge_Y[R_edge_Num];
				Search_8_mid_Num_Will = 9 - Search_8_REM;
			}
			else
			{
				IF_Search_8_Continue = 0;
				flag_type_R = 1;
				if (edge_X <= IMG_W - Num_search_UP_Turn2)
				{
					flag_type_R = 2;
				}
				if (edge_X >= IMG_W - Num_search_UP_Turn1)
				{
					flag_type_R = 3;
				}
			}
		}
		if (edge_X < Num_lim_search_LR)//从左边出去
		{
			IF_Search_8_Continue = 0;
			flag_type_R = 4;
			if (edge_Y < Num_search_LR_Turn2)
			{
				flag_type_R = 5;
			}
			if (edge_Y < Num_search_LR_Turn1)
			{
				flag_type_R = 6;
			}
		}
		if (edge_X >= IMG_W - Num_lim_search_LR)//从右边出去
		{
			IF_Search_8_Continue = 0;
			flag_type_R = 7;
			if (edge_Y < Num_search_LR_Turn2)
			{
				flag_type_R = 8;
			}
			if (edge_Y < Num_search_LR_Turn1)
			{
				flag_type_R = 9;
			}
		}
		if (edge_Y < Num_up_search_skip)
		{
			IF_Search_8_Continue = 0;
			if (edge_X > IMG_mid_R)
			{
				flag_type_R = 9;
			}
			else
			{
				flag_type_R = 6;
			}
		}
	}
}

//刚开始的扫描图像下面的中间为黑色
uint8 Search_Start_MidIsBlack(void)
{
	flag_temp_ = 1;
	flag_type = 2;
	uint8 L_NUM = 0;
	uint8 R_NUM = 0;
	for (uint8 i = 0; i < IMG_mid_L; i++)
	{
		if (img[Search_start_Y_L][IMG_mid_L - i] == White)
		{
			L_NUM++;
		}
	}
	for (uint8 i = 0; i < IMG_W - IMG_mid_R; i++)
	{
		if (img[Search_start_Y_R][IMG_mid_R + i] == White)
		{
			R_NUM++;
		}
	}
	if (L_NUM < R_NUM)
	{
		flag_Search_8_RL = 1;
		flag_type_curve = 2;
		flag_type_R = 10;
		for (uint8 i = 0; i < IMG_W - IMG_mid_L; i++)
		{
			if (img[Search_start_Y_L][IMG_mid_L + i] == White)
			{
				edge_X = IMG_mid_L + i - 1;
				edge_Y = Search_start_Y_L;
				Put_Data_to_L_edge(edge_X, edge_Y);
				Search_8_mid_Num_Will = 0;
				Search_8();
				if (IF_Warning_MOD)
				{
					return 0;
				}
				Search_8_mid_Num_Will_rem[1] = Search_8_mid_Num_Will;
				break;
			}
		}
	}
	else
	{
		flag_Search_8_RL = 2;
		flag_type_curve = 1;
		flag_type_L = 10;
		for (uint8 i = 0; i < IMG_mid_R; i++)
		{
			if (img[Search_start_Y_R][IMG_mid_R - i] == White)
			{
				edge_X = IMG_mid_R - i + 1;
				edge_Y = Search_start_Y_R;
				Put_Data_to_R_edge(edge_X, edge_Y);
				Search_8_mid_Num_Will = 0;
				Search_8();
				if (IF_Warning_MOD)
				{
					return 0;
				}
				Search_8_mid_Num_Will_rem[2] = Search_8_mid_Num_Will;
				break;
			}
		}
	}
	return 1;
}

//寻找边沿起始点（左），正常flag=1
uint8 Search_for_L_start_point(uint8 flag)
{//********************************************************************************************************************************************DELFOR
	if (img[Search_start_Y_L][IMG_mid_L] == Black && img[Search_start_Y_R][IMG_mid_R] == Black && flag)
	{
		Search_Start_MidIsBlack();
	}
	else
	{
		Search_to_L(flag);
		if (flag_type_L != 10)
		{
			Search_8_mid_Num_Will = 0;
			if (!flag)
			{
				flag_Search_8_again = 1;
			}
			Search_8();
			if (IF_Warning_MOD)
			{
				return 0;
			}
			flag_Search_8_again = 0;
			Search_start_Y_L++;
			if (img[Search_start_Y_L][IMG_mid_L] == Black && img[Search_start_Y_R][IMG_mid_R] == Black && flag)
			{
				Search_Start_MidIsBlack();
			}
			else
			{
				Search_to_L(flag);
			}
			if (IF_A_B_equal(1, L_edge_Num, L_edge_Num - 1))
			{
				Del_Data_Now_L();
				Search_start_Y_L--;
				flag_Event_type = 0;
				Search_8_mid_Num_Will_rem[1] = Search_8_mid_Num_Will;
			}
			else
			{
				Del_Data_from_N_L(1);
				Search_for_L_start_point(1);
			}
		}
	}
	return 1;
}

//寻找边沿起始点（右），正常flag=1
uint8 Search_for_R_start_point(uint8 flag)
{
	if (img[Search_start_Y_L][IMG_mid_L] == Black && img[Search_start_Y_R][IMG_mid_R] == Black && flag)
	{
		Search_Start_MidIsBlack();
	}
	else
	{
		Search_to_R(flag);
		if (flag_type_R != 10)
		{
			Search_8_mid_Num_Will = 0;
			if (!flag)
			{
				flag_Search_8_again = 1;
			}
			Search_8();
			if (IF_Warning_MOD)
			{
				return 0;
			}
			flag_Search_8_again = 0;
			Search_start_Y_R++;
			if (img[Search_start_Y_L][IMG_mid_L] == Black && img[Search_start_Y_R][IMG_mid_R] == Black && flag)
			{
				Search_Start_MidIsBlack();
			}
			else
			{
				Search_to_R(flag);
			}
			if (IF_A_B_equal(0, R_edge_Num, R_edge_Num - 1))
			{
				Del_Data_Now_R();
				Search_start_Y_R--;
				flag_Event_type = 0;
				Search_8_mid_Num_Will_rem[2] = Search_8_mid_Num_Will;
			}
			else
			{
				Del_Data_from_N_R(1);
				Search_for_R_start_point(1);
			}
		}
	}
	return 1;
}

//向上搜索跳变点
uint8 Search_to_UP(uint8 X, uint8 Y)
{
	Y = Y + Num_Search_to_UP_skip;
	uint8 NUM_REM = 0;
	uint8 NUM_White = 0;
	for (uint8 i = Y; i < IMG_H; i++)
	{
		NUM_REM = 0;
		NUM_White = 0;
		if (img[i][X] == Black)
		{
			for (uint8 j = 1; j < SearchToUPMaxSearchNum; j++)
			{
				if (i + j < IMG_H)
				{
					NUM_REM++;
				}
			}
			for (uint8 j = NUM_REM; j > 0; j--)
			{
				if (img[i + j][X] == Black)
				{
					NUM_White++;
				}
			}
			if (!NUM_REM)
			{
				Y = i;
				X = 250;
				break;
			}
			if (NUM_White > 1)
			{
				Y = i;
				X = 250;
				break;
			}
		}
	}
	if (X == 250)
	{
		return Y;
	}
	else
	{
		return 0;
	}
}

//向上寻找十字路的下一个转折点（左）
void Croos_find_turn_point_U_L(uint8 Start_point_Num)
{
	uint8 flag_All_White = 0;
	for (uint8 i = L_edge_Num; i >= Start_point_Num; i--)
	{
		flag_All_White = 1;
		for (uint8 j = 0; j <= Num_Croos_find_turn_point; j++)
		{
			if (img[L_edge_Y[i] - 1][L_edge_X[i] - j] == Black)
			{
				flag_All_White = 0;
			}
		}
		if (flag_All_White)
		{
			Croos_turn_point[5] = L_edge_X[i];
			Croos_turn_point[6] = L_edge_Y[i];
			Croos_turn_point_Num[3] = i;
			flag_All_White = 2;
			break;
		}
	}
	if (flag_All_White != 2)
	{
		Croos_turn_point[5] = L_edge_X[Start_point_Num];
		Croos_turn_point[6] = L_edge_Y[Start_point_Num];
		Croos_turn_point_Num[3] = Start_point_Num;
		flag_All_White = 2;
	}
}

//向上寻找十字路的下一个转折点（右）
void Croos_find_turn_point_U_R(uint8 Start_point_Num)
{
	uint8 flag_All_White = 0;
	for (uint8 i = R_edge_Num; i >= Start_point_Num; i--)
	{
		flag_All_White = 1;
		for (uint8 j = 0; j <= Num_Croos_find_turn_point; j++)
		{
			if (img[R_edge_Y[i] - 1][R_edge_X[i] + j] == Black)
			{
				flag_All_White = 0;
			}
		}
		if (flag_All_White)
		{
			Croos_turn_point[7] = R_edge_X[i];
			Croos_turn_point[8] = R_edge_Y[i];
			Croos_turn_point_Num[4] = i;
			flag_All_White = 2;
			break;
		}
	}
	if (flag_All_White != 2)
	{
		Croos_turn_point[7] = R_edge_X[Start_point_Num];
		Croos_turn_point[8] = R_edge_Y[Start_point_Num];
		Croos_turn_point_Num[4] = Start_point_Num;
		flag_All_White = 2;
	}
}

//两个点连接并输入到边界数组中（左,X1<X2,Y1<Y2）,XY2在数组中，插入到第AIM个
uint8 TWO_Point_Get_edge_L(uint8 X1, uint8 Y1, uint8 X2, uint8 Y2, uint8 AIM)
{
	if (X1 >= IMG_W || Y1 >= IMG_H || X2 >= IMG_W || Y2 >= IMG_H)
	{
		Check(1);
		IF_Warning_MOD = 1;
		return 0;
	}
	int16 number_X = X2 - X1 - 1;
	int16 number_Y = Y2 - Y1 - 1;
	uint8 abs_X,abs_Y;
	int16 X3;
	int16 Y3;
	float K = 0;
	if (!number_X || number_X >= IMG_W - 2 || number_X <= -IMG_W + 2)
	{
		number_X = 1;
	}
	if (!number_Y || number_Y >= IMG_H - 2 || number_Y <= -IMG_H + 2)
	{
		number_Y = 1;
	}
	if (number_X < 0)
	{
		abs_X = -number_X;
	}
	else
	{
		abs_X = number_X;
	}
	if (number_Y < 0)
	{
		abs_Y = -number_Y;
	}
	else
	{
		abs_Y = number_Y;
	}
	if (L_edge_Num + abs_X < Num_edge - 2 && L_edge_Num + abs_Y < Num_edge - 2)
	{
		if (abs_X < abs_Y)
		{
			K = (float)number_X / (float)number_Y;
			if (K < 0) K = -K;
			for (int8 i = L_edge_Num - AIM + 1; i >= 0; i--)
			{
				L_edge_X[AIM + i + abs_Y] = L_edge_X[AIM + i];
				L_edge_Y[AIM + i + abs_Y] = L_edge_Y[AIM + i];
			}
			L_edge_Num += abs_Y;
			Croos_turn_point_Num[3] += abs_Y;
			if (abs_Y == 1)
			{
				if (L_edge_Y[AIM + 1] == Y1)
				{
					L_edge_X[AIM] = X2;
					L_edge_Y[AIM] = Y2;
				}
				if (L_edge_Y[AIM + 1] == Y2)
				{
					L_edge_X[AIM] = X1;
					L_edge_Y[AIM] = Y1;
				}
			}
			else
			{
				for (uint8 i = 1; i <= abs_Y; i++)
				{
					X3 = Round_self((float)i * K);
					L_edge_X[AIM + i - 1] = X1 + X3;
					if (number_Y > 0)
					{
						L_edge_Y[AIM + i - 1] = Y1 + i;
					}
					else
					{
						L_edge_Y[AIM + i - 1] = Y1 - i;
					}
					if (L_edge_X[AIM + i - 1] >= IMG_W || L_edge_Y[AIM + i - 1] >= IMG_H)
					{
						L_edge_X[AIM + i - 1] = 0;
						L_edge_Y[AIM + i - 1] = 0;
						break;
					}
				}
			}
		}
		else
		{
			K = (float)number_Y / (float)number_X;
			if (K < 0) K = -K;
			for (int8 i = L_edge_Num - AIM + 1; i >= 0; i--)
			{
				L_edge_X[AIM + i + abs_X] = L_edge_X[AIM + i];
				L_edge_Y[AIM + i + abs_X] = L_edge_Y[AIM + i];
			}
			L_edge_Num += abs_X;
			Croos_turn_point_Num[3] += abs_X;
			if (abs_X == 1)
			{
				if (L_edge_Y[AIM + 1] == Y1 || L_edge_Y[AIM - 1] == Y1)
				{
					L_edge_X[AIM] = X2;
					L_edge_Y[AIM] = Y2;
				}
				if (L_edge_Y[AIM + 1] == Y2 || L_edge_Y[AIM - 1] == Y2)
				{
					L_edge_X[AIM] = X1;
					L_edge_Y[AIM] = Y1;
				}
			}
			else
			{
				for (uint8 i = 1; i <= abs_X; i++)
				{
					Y3 = Round_self((float)i * K);
					if (number_X > 0)
					{
						L_edge_X[AIM + i - 1] = X1 + i;
					}
					else
					{
						L_edge_X[AIM + i - 1] = X1 - i;
					}
					L_edge_Y[AIM + i - 1] = Y1 + Y3;
					if (L_edge_X[AIM + i - 1] >= IMG_W || L_edge_Y[AIM + i - 1] >= IMG_H)
					{
						L_edge_X[AIM + i - 1] = 0;
						L_edge_Y[AIM + i - 1] = 0;
						break;
					}
				}
			}
		}
	}
	return 0;
}

//两个点连接并输入到边界数组中（右,X2<X1,Y1<Y2）,XY2之前未添加到数组中
uint8 TWO_Point_Get_edge_R(uint8 X1, uint8 Y1, uint8 X2, uint8 Y2, uint8 AIM)
{
	if (X1 >= IMG_W || Y1 >= IMG_H || X2 >= IMG_W || Y2 >= IMG_H)
	{
		Check(0);
		IF_Warning_MOD = 1;
		return 0;
	}
	int16 number_X = X1 - X2 - 1;
	int16 number_Y = Y2 - Y1 - 1;
	uint8 abs_X, abs_Y;
	int16 X3;
	int16 Y3;
	float K = 0;
	if (!number_X || number_X >= IMG_W - 2 || number_X <= -IMG_W + 2)
	{
		number_X = 1;
	}
	if (!number_Y || number_Y >= IMG_H - 2 || number_Y <= -IMG_H + 2)
	{
		number_Y = 1;
	}
	if (number_X < 0)
	{
		abs_X = -number_X;
	}
	else
	{
		abs_X = number_X;
	}
	if (number_Y < 0)
	{
		abs_Y = -number_Y;
	}
	else
	{
		abs_Y = number_Y;
	}
	if (R_edge_Num + abs_X < Num_edge - 2 && R_edge_Num + abs_Y < Num_edge - 2)
	{
		if (abs_X < abs_Y)
		{
			K = (float)number_X / (float)number_Y;
			if (K < 0) K = -K;
			for (int8 i = R_edge_Num - AIM + 1; i >= 0; i--)
			{
				R_edge_X[AIM + i + abs_Y] = R_edge_X[AIM + i];
				R_edge_Y[AIM + i + abs_Y] = R_edge_Y[AIM + i];
			}
			R_edge_Num += abs_Y;
			Croos_turn_point_Num[4] += abs_Y;
			if (abs_Y == 1)
			{
				if (R_edge_Y[AIM + 1] == Y1 || R_edge_Y[AIM - 1] == Y1)
				{
					R_edge_X[AIM] = X2;
					R_edge_Y[AIM] = Y2;
				}
				if (R_edge_Y[AIM + 1] == Y2 || R_edge_Y[AIM - 1] == Y2)
				{
					R_edge_X[AIM] = X1;
					R_edge_Y[AIM] = Y1;
				}
			}
			else
			{
				for (uint8 i = 1; i <= abs_Y; i++)
				{
					X3 = Round_self((float)i * K);
					R_edge_X[AIM + i - 1] = X1 - X3;
					if (number_Y > 0)
					{
						R_edge_Y[AIM + i - 1] = Y1 + i;
					}
					else
					{
						R_edge_Y[AIM + i - 1] = Y1 - i;
					}
					if (R_edge_X[AIM + i - 1] >= IMG_W || R_edge_Y[AIM + i - 1] >= IMG_H)
					{
						R_edge_X[AIM + i - 1] = IMG_W - Num_lim_search_LR;
						R_edge_Y[AIM + i - 1] = 0;
						break;
					}
				}
			}
		}
		else
		{
			K = (float)number_Y / (float)number_X;
			if (K < 0) K = -K;
			for (int8 i = R_edge_Num - AIM + 1; i >= 0; i--)
			{
				R_edge_X[AIM + i + abs_X] = R_edge_X[AIM + i];
				R_edge_Y[AIM + i + abs_X] = R_edge_Y[AIM + i];
			}
			R_edge_Num += abs_X;
			Croos_turn_point_Num[4] += abs_X;
			if (abs_X == 1)
			{
				if (R_edge_Y[AIM + 1] == Y1)
				{
					R_edge_X[AIM] = X2;
					R_edge_Y[AIM] = Y2;
				}
				if (R_edge_Y[AIM + 1] == Y2)
				{
					R_edge_X[AIM] = X1;
					R_edge_Y[AIM] = Y1;
				}
			}
			else
			{
				for (uint8 i = 1; i <= abs_X; i++)
				{
					Y3 = Round_self((float)i * K);
					if (number_X > 0)
					{
						R_edge_X[AIM + i - 1] = X1 - i;
					}
					else
					{
						R_edge_X[AIM + i - 1] = X1 + i;
					}
					R_edge_Y[AIM + i - 1] = Y1 + Y3;
					if (R_edge_X[AIM + i - 1] >= IMG_W || R_edge_Y[AIM + i - 1] >= IMG_H)
					{
						R_edge_X[AIM + i - 1] = IMG_W - Num_lim_search_LR;
						R_edge_Y[AIM + i - 1] = 0;
						break;
					}
				}
			}
		}
	}
	return 0;
}

//十字路向上搜索并进行扫描（左）
void Croos_Search_up_L2(void)
{
	uint8 Plus = Search_up_NOedge_30_Move;
	//向上寻找跳变点
	if (!L_edge_Num)
	{
		L_edge_X[0] = Cross_3_Make_edge;
		L_edge_Y[0] = Search_to_UP(Cross_3_Make_edge, Num_Search_to_UP_skip);
		while (!L_edge_Y[0] && Plus)
		{
			L_edge_Y[0] = Search_to_UP(Cross_3_Make_edge + Search_up_NOedge_30_Move + 1 - Plus, Num_Search_to_UP_skip);
			L_edge_X[0] = Cross_3_Make_edge + Search_up_NOedge_30_Move + 1 - Plus;
			Plus--;
		}
		if (L_edge_X[0] && L_edge_Y[0])
		{
			Put_Data_to_L_edge(L_edge_X[0], L_edge_Y[0]);
		}
	}
	else
	{
		L_edge_Y[0] = Search_to_UP(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
	}
	if (!L_edge_Y[0])
	{
		Cross_L_IF_2 = 0;
	}
	else
	{
		Cross_L_IF_2 = 1;
	}
}

//十字路向上搜索并进行扫描（右）
void Croos_Search_up_R2(void)
{
	uint8 Plus = Search_up_NOedge_30_Move;
	//向上寻找跳变点
	if (!R_edge_Num)
	{
		R_edge_X[0] = IMG_W - 1 - Cross_3_Make_edge;
		R_edge_Y[0] = Search_to_UP(IMG_W - 1 - Cross_3_Make_edge, Num_Search_to_UP_skip);
		while (!R_edge_Y[0] && Plus)
		{
			R_edge_Y[0] = Search_to_UP(IMG_W - 1 - Cross_3_Make_edge - Search_up_NOedge_30_Move - 1 + Plus, Num_Search_to_UP_skip);
			R_edge_X[0] = IMG_W - 1 - Cross_3_Make_edge - Search_up_NOedge_30_Move - 1 + Plus;
			Plus--;
		}
		if (R_edge_X[0] && R_edge_Y[0])
		{
			Put_Data_to_R_edge(R_edge_X[0], R_edge_Y[0]);
		}
	}
	else
	{
		R_edge_Y[0] = Search_to_UP(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
	}
	if (!R_edge_Y[0])
	{
		Cross_R_IF_2 = 0;
	}
	else
	{
		Cross_R_IF_2 = 1;
	}
}

//左边沿扫描
uint8 Search_8_L(uint8 X_Start_point, uint8 Y_Start_point)
{
	//特殊权限
	if (flag_type == 3 && flag_type_cross < 4)
	{
		flag_Search_8_again = 1;
	}
	if (flag_type == 5)
	{
		flag_Search_8_again = 1;
	}
	//扫描
	//搜索8初始化
	Search_8_mid_Num_Will = 0;
	IF_Search_8_Continue = 1;
	flag_Search_8_RL = 1;
	edge_X = X_Start_point;
	edge_Y = Y_Start_point;
	//进行扫描，左边沿
	while (IF_Search_8_Continue)
	{
		Search_8();
		if (IF_Warning_MOD)
		{
			return 0;
		}
		Judge();
	}
	flag_Search_8_again = 0;
	return 1;
}

//右边沿扫描
uint8 Search_8_R(uint8 X_Start_point, uint8 Y_Start_point)
{
	//特殊权限
	if (flag_type == 3 && flag_type_cross < 4)
	{
		flag_Search_8_again = 1;
	}
	if (flag_type == 5)
	{
		flag_Search_8_again = 1;
	}
	//扫描
	//搜索8初始化
	Search_8_mid_Num_Will = 0;
	IF_Search_8_Continue = 1;
	flag_Search_8_RL = 2;
	edge_X = X_Start_point;
	edge_Y = Y_Start_point;

	//进行扫描，右边沿
	while (IF_Search_8_Continue)
	{
		Search_8();
		if (IF_Warning_MOD)
		{
			return 0;
		}
		Judge();
	}
	flag_Search_8_again = 0;
	return 1;
}

//无上方两个转折点，计算斜率，向上拟定边界
uint8 Get_K_To_get_UP_edge(uint8 L_or_R)
{
	if (IF_Warning_MOD)
	{
		return 0;
	}
	int16 X = 0;
	int16 Y = 0;
	uint8 flag = 0;
	Mid_Num++;
	if (L_or_R && L_edge_Num < Num_edge - 5 && Mid_Num < 20)
	{
		Y = IMG_H - L_edge_Y[L_edge_Num] - 1;
		for (uint8 i = 0; i < L_edge_Num; i++)
		{
			if (L_edge_Y[L_edge_Num - i] == L_edge_Y[L_edge_Num] - Y)
			{
				X = L_edge_X[L_edge_Num] - L_edge_X[L_edge_Num - i];
				if (L_edge_X[L_edge_Num] > IMG_W - X)
				{
					X = IMG_W - 1 - L_edge_X[L_edge_Num];
				}
				TWO_Point_Get_edge_L(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num], L_edge_X[L_edge_Num] + X, IMG_H - 1, L_edge_Num + 1);
				flag = 1;
				break;
			}
		}
		if (!flag)
		{
			Y = L_edge_Y[L_edge_Num] - L_edge_Y[1];
			X = L_edge_X[L_edge_Num] - L_edge_X[1];
			if (L_edge_X[L_edge_Num] > IMG_W - X)
			{
				X = IMG_W - 1 - L_edge_X[L_edge_Num];
			}
			if (L_edge_Y[L_edge_Num] > IMG_H - Y)
			{
				Y = IMG_H - 1 - L_edge_Y[L_edge_Num];
			}
			TWO_Point_Get_edge_L(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num], L_edge_X[L_edge_Num] + X, L_edge_Y[L_edge_Num] + Y, L_edge_Num + 1);
			Get_K_To_get_UP_edge(1);
		}
	}
	else if (!L_or_R && R_edge_Num < Num_edge - 5 && Mid_Num < 20)
	{
		Y = IMG_H - R_edge_Y[R_edge_Num] - 1;
		for (uint8 i = 0; i < R_edge_Num; i++)
		{
			if (R_edge_Y[R_edge_Num - i] == R_edge_Y[R_edge_Num] - Y)
			{
				X = R_edge_X[R_edge_Num - i] - R_edge_X[R_edge_Num];
				if (R_edge_X[R_edge_Num] < X)
				{
					X = R_edge_X[R_edge_Num];
				}
				TWO_Point_Get_edge_R(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num], R_edge_X[R_edge_Num] - X, IMG_H - 1, R_edge_Num + 1);
				flag = 1;
				break;
			}
		}
		if (!flag)
		{
			Y = R_edge_Y[R_edge_Num] - R_edge_Y[1];
			X = R_edge_X[1] - R_edge_X[R_edge_Num];
			if (R_edge_X[R_edge_Num] < X)
			{
				X = R_edge_X[R_edge_Num];
			}
			if (R_edge_Y[R_edge_Num] > IMG_H - Y)
			{
				Y = IMG_H - 1 - R_edge_Y[R_edge_Num];
			}
			TWO_Point_Get_edge_R(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num], R_edge_X[R_edge_Num] - X, R_edge_Y[R_edge_Num] + Y, R_edge_Num + 1);
			Get_K_To_get_UP_edge(0);
		}
	}
	Mid_Num = 0;
	return 0;
}

//无下方两个转折点，计算斜率，向下拟定边界
uint8 Get_K_To_get_Down_edge(uint8 L_or_R)
{
	if (IF_Warning_MOD)
	{
		return 0;
	}
	int16 X = 0;
	int16 Y = 0;
	uint8 flag = 0;
	Mid_Num++;
	if (L_or_R && L_edge_Num < Num_edge - 5 && Mid_Num < 20)
	{
		Y = L_edge_Y[1];
		for (uint8 i = 1; i <= L_edge_Num; i++)
		{
			if (L_edge_Y[i] - Y == Y)
			{
				X = L_edge_X[i] - L_edge_X[1];
				if (L_edge_X[1] < X)
				{
					X = L_edge_X[1];
				}
				TWO_Point_Get_edge_L(L_edge_X[1] - X, 1, L_edge_X[1], L_edge_Y[1], 1);
				flag = 1;
				break;
			}
		}
		if (!flag)
		{
			Y = L_edge_Y[L_edge_Num] - L_edge_Y[1];
			X = L_edge_X[L_edge_Num] - L_edge_X[1];
			if (L_edge_X[1] < X)
			{
				X = L_edge_X[1];
			}
			if (L_edge_Y[1] < Y)
			{
				Y = L_edge_Y[1];
			}
			TWO_Point_Get_edge_L(L_edge_X[1] - X, L_edge_Y[1] - Y, L_edge_X[1], L_edge_Y[1], 1);
			Get_K_To_get_Down_edge(1);
		}
	}
	else if (!L_or_R && R_edge_Num < Num_edge - 5 && Mid_Num < 20)
	{
		Y = R_edge_Y[1];
		for (uint8 i = 1; i <= R_edge_Num; i++)
		{
			if (R_edge_Y[i] - Y == Y)
			{
				X = R_edge_X[1] - R_edge_X[i];
				if (R_edge_X[1] > IMG_W - X)
				{
					X = IMG_W - R_edge_X[1] - 1;
				}
				TWO_Point_Get_edge_R(R_edge_X[1] + X, 1, R_edge_X[1], R_edge_Y[1], 1);
				flag = 1;
				break;
			}
		}
		if (!flag)
		{
			Y = R_edge_Y[R_edge_Num] - R_edge_Y[1];
			X = R_edge_X[1] - R_edge_X[R_edge_Num];
			if (R_edge_X[1] > IMG_W - X)
			{
				X = IMG_W - R_edge_X[1] - 1;
			}
			if (R_edge_Y[1] < Y)
			{
				Y = R_edge_Y[1];
			}
			TWO_Point_Get_edge_R(R_edge_X[1] + X, R_edge_Y[1] - Y, R_edge_X[1], R_edge_Y[1], 1);
			Get_K_To_get_Down_edge(0);
		}
	}
	Mid_Num = 0;
	return 0;
}

//白色通路计算
uint8 How_many_White_route(uint8 Y_coordinate)
{
	uint8 Is_Black = 1;
	uint8 Num_Consecutive_Block = 0;
	uint8 Num_route = 0;
	uint8 Num_Black_Block = 0;
	for (uint8 X_coordinate = 0; X_coordinate < IMG_W; X_coordinate++)
	{
		if (img[Y_coordinate][X_coordinate] == White && Is_Black)
		{
			Num_Consecutive_Block++;
			if (Num_Consecutive_Block >= NUM_block_route)
			{
				Is_Black = 0;
				Num_route++;
				Num_Consecutive_Block = 0;
			}
		}
		if (img[Y_coordinate][X_coordinate] == Black)
		{
			Num_Black_Block++;
			if (!Is_Black)
			{
				Num_Consecutive_Block++;
				if (Num_Consecutive_Block >= NUM_block_route)
				{
					Is_Black = 1;
					Num_Consecutive_Block = 0;
				}
			}
		}
	}
	if (Num_Black_Block < 2)
	{
		Num_route = 3;
	}
	if (Num_route == 1 && Num_Black_Block > IMG_W/2)
	{
		Num_route = 10;
	}
	if (Num_route == 2 && Num_Black_Block > IMG_W * 0.25)
	{
		Num_route = 30;
		for (uint8 i = 1; i <= 5; i++)
		{
			if (img[Y_coordinate][i] == Black)
			{
				Num_route = 20;
				break;
			}
			if (img[Y_coordinate][IMG_W - i] == Black)
			{
				Num_route = 20;
				break;
			}
		}
	}
	return Num_route;
}

//黑色通路计算,返回10和20为特殊
uint8 How_many_Black_route(uint8 Y_coordinate)
{
	uint8 Is_White = 1;
	uint8 Num_Consecutive_Block = 0;
	uint8 Num_route = 0;
	uint8 Num_Black_Block = 0;
	for (uint8 X_coordinate = 0; X_coordinate < IMG_W; X_coordinate++)
	{
		if (img[Y_coordinate][X_coordinate] == Black)
		{
			Num_Black_Block++;
			if (Is_White)
			{
				Num_Consecutive_Block++;
				if (Num_Consecutive_Block >= NUM_block_route)
				{
					Is_White = 0;
					Num_route++;
					Num_Consecutive_Block = 0;
				}
			}
		}
		if (img[Y_coordinate][X_coordinate] == White && !Is_White)
		{
			Num_Consecutive_Block++;
			if (Num_Consecutive_Block >= NUM_block_route)
			{
				Is_White = 1;
				Num_Consecutive_Block = 0;
			}
		}
	}
	if (Num_Black_Block < 10 && Num_route == 1)
	{
		Num_route = 10;
	}
	if (Num_Black_Block < 10 && Num_route == 2)
	{
		Num_route = 20;
	}
	return Num_route;
}

//上方寻找右点并添加至数组
void Find_R_UP_point(uint8 Start_X, uint8 Start_Y)
{
	uint8 Temp = 1;
	Cross_R_IF_2 = 0;
	Start_X += NUM_block_route;
	for (uint8 i = Start_X; i < IMG_W - Num_lim_search_LR - 1; i++)
	{
		if (img[Start_Y][i] == Black)
		{
			if (i == Start_X)
			{
				Temp = 0;
			}
			if (Temp)
			{
				Cross_R_IF_2 = 1;
				for (uint8 j = 1; j < NUM_block_route; j++)
				{
					if (img[Start_Y][i + j] != Black)
					{
						Cross_R_IF_2 = 0;
						break;
					}
				}
				if (Cross_R_IF_2)
				{
					Put_Data_to_R_edge(i, Start_Y);
					break;
				}
			}
			else
			{
				if (img[Start_Y][i + 1] == White)
				{
					Temp = 1;
				}
			}
		}
	}
}

//上方寻找左点并添加至数组
void Find_L_UP_point(uint8 Start_X, uint8 Start_Y)
{
	uint8 Temp = 1;
	Cross_L_IF_2 = 0;
	Start_X -= NUM_block_route;
	for (uint8 i = Start_X; i > Num_lim_search_LR; i--)
	{
		if (img[Start_Y][i] == Black)
		{
			if (i == Start_X)
			{
				Temp = 0;
			}
			if (Temp)
			{
				Cross_L_IF_2 = 1;
				for (uint8 j = 1; j < NUM_block_route; j++)
				{
					if (img[Start_Y][i - j] != Black)
					{
						Cross_L_IF_2 = 0;
						break;
					}
				}
				if (Cross_L_IF_2)
				{
					Put_Data_to_L_edge(i, Start_Y);
					break;
				}
			}
			else
			{
				if (img[Start_Y][i - 1] == White)
				{
					Temp = 1;
				}
			}
		}
	}
}

//上方寻找右点并添加至数组
void Find_R_UP_point_W(uint8 Start_X, uint8 Start_Y)
{
	Cross_R_IF_2 = 0;
	Start_X += NUM_block_route;
	for (uint8 i = Start_X; i < IMG_W - Num_lim_search_LR - 1; i++)
	{
		if (img[Start_Y][i] == White)
		{
			Cross_R_IF_2 = 1;
			for (uint8 j = 1; j < NUM_block_route; j++)
			{
				if (img[Start_Y][i + j] != White)
				{
					Cross_R_IF_2 = 0;
					break;
				}
			}
			if (Cross_R_IF_2)
			{
				Put_Data_to_R_edge(i + 1, Start_Y);
				break;
			}
		}
	}
}

//上方寻找左点并添加至数组
void Find_L_UP_point_W(uint8 Start_X, uint8 Start_Y)
{
	Cross_L_IF_2 = 0;
	Start_X -= NUM_block_route;
	for (uint8 i = Start_X; i > Num_lim_search_LR; i--)
	{
		if (img[Start_Y][i] == White)
		{
			Cross_L_IF_2 = 1;
			for (uint8 j = 1; j < NUM_block_route; j++)
			{
				if (img[Start_Y][i - j] != White)
				{
					Cross_L_IF_2 = 0;
					break;
				}
			}
			if (Cross_L_IF_2)
			{
				Put_Data_to_L_edge(i - 1, Start_Y);
				break;
			}
		}
	}
}

//环岛处理程序核心
void Round_main(void)//11以后出问题
{
	uint8 Temp_A, Temp_B;
	if (flag_type_round <= 10)
	{
		if (flag_type_round % 2)
		{
			if (flag_type_Number == 95 || flag_type_Number == 94 || flag_type_Number == 93)
			{
				flag_type_round = 9;
			}
		}
		else
		{
			if (flag_type_Number == 89 || flag_type_Number == 79 || flag_type_Number == 69)
			{
				flag_type_round = 10;
			}
		}
	}
	//判断环岛阶段
	switch (flag_type_round)
	{
	case 0:
		break;
	case 1:
		//环岛边沿扫描没找到（左环岛）
		if (flag_type_R == 10)
		{
			God_FUCK_me(0);
			flag_type_R = 1;
		}
		if (flag_type_L == 10)
		{
			flag_type_round = 3;
		}
		break;
	case 2:
		//环岛边沿扫描没找到（右环岛）
		if (flag_type_L == 10)
		{
			God_FUCK_me(1);
			flag_type_L = 1;
		}
		if (flag_type_R == 10)
		{
			flag_type_round = 4;
		}
		break;
	case 3:
		//环岛边沿扫描没找到（左环岛）
		if (flag_type_R == 10)
		{
			God_FUCK_me(0);
			flag_type_R = 1;
		}
		if (flag_type_L != 10)
		{
			flag_type_round = 5;
		}
		break;
	case 4:
		//环岛边沿扫描没找到（右环岛）
		if (flag_type_L == 10)
		{
			God_FUCK_me(1);
			flag_type_L = 1;
		}
		if (flag_type_R != 10)
		{
			flag_type_round = 6;
		}
		break;
	case 5://直道没边（左环岛）
		Temp_A = 0;
		if (flag_type_R == 10)
		{
			for (uint8 i = IMG_W - 6; i < IMG_W - 2; i++)
			{
				for (uint8 j = IMG_H - 4; j < IMG_H; j++)
				{
					if (img[j][i])
					{
						Temp_A++;
					}
				}
			}
		}
		if (Temp_A > 10)
		{
			flag_type_round = 7;
		}
		Get_angle_R(1, R_edge_Num);
		if (R_edge_Num > 6)
		{
			Temp_A = 0;
			R_edge_angle[0] = 0;
			for (uint8 i = 4; i < R_edge_Num - 2; i++)
			{
				if (R_edge_angle[Temp_A] < R_edge_angle[i])
				{
					Temp_A = i;
				}
			}
			if (R_edge_angle[Temp_A] > angle_is_S)
			{
				R_edge_angle[0] = Temp_A;
				flag_type_round = 11;
			}
		}
		break;
	case 6://直道没边（右环岛）
		Temp_A = 0;
		if (flag_type_L == 10)
		{
			for (uint8 i = 1; i < 5; i++)
			{
				for (uint8 j = IMG_H - 4; j < IMG_H; j++)
				{
					if (img[j][i])
					{
						Temp_A++;
					}
				}
			}
		}
		if (Temp_A > 10)
		{
			flag_type_round = 8;
		}
		Get_angle_L(1, L_edge_Num);
		if (L_edge_Num > 6)
		{
			Temp_A = 0;
			L_edge_angle[0] = 0;
			for (uint8 i = 4; i < L_edge_Num - 2; i++)
			{
				if (L_edge_angle[Temp_A] < L_edge_angle[i])
				{
					Temp_A = i;
				}
			}
			if (L_edge_angle[Temp_A] > angle_is_S)
			{
				L_edge_angle[0] = Temp_A;
				flag_type_round = 12;
			}
		}
		break;
	case 7://进入急转弯（左环岛）
		if (flag_type_R != 10)
		{
			flag_type_round = 9;
		}
		break;
	case 8://进入急转弯（右环岛）
		if (flag_type_L != 10)
		{
			flag_type_round = 10;
		}
		break;
	case 9://右边出现锐角（左环岛）
		Get_angle_R(1, R_edge_Num);
		if (R_edge_Num > 6)
		{
			Temp_A = 0;
			R_edge_angle[0] = 0;
			for (uint8 i = 4; i < R_edge_Num - 2; i++)
			{
				if (R_edge_angle[Temp_A] < R_edge_angle[i])
				{
					Temp_A = i;
				}
			}
			if (R_edge_angle[Temp_A] > angle_is_S)
			{
				R_edge_angle[0] = Temp_A;
				flag_type_round = 11;
			}
		}
		break;
	case 10://左边出现锐角（右环岛）
		Get_angle_L(1, L_edge_Num);
		if (L_edge_Num > 6)
		{
			Temp_A = 0;
			L_edge_angle[0] = 0;
			for (uint8 i = 4; i < L_edge_Num - 2; i++)
			{
				if (L_edge_angle[Temp_A] < L_edge_angle[i])
				{
					Temp_A = i;
				}
			}
			if (L_edge_angle[Temp_A] > angle_is_S)
			{
				L_edge_angle[0] = Temp_A;
				flag_type_round = 12;
			}
		}
		break;
	case 11://右边界没找到（左环岛）
		if (flag_type_R == 10)
		{
			flag_type_round = 13;
		}
		else
		{
			Get_angle_R(1, R_edge_Num);
			if (R_edge_Num > 6)
			{
				Temp_A = 0;
				R_edge_angle[0] = 0;
				for (uint8 i = 4; i < R_edge_Num - 2; i++)
				{
					if (R_edge_angle[Temp_A] < R_edge_angle[i])
					{
						Temp_A = i;
					}
				}
				if (R_edge_angle[Temp_A] > angle_is_S)
				{
					R_edge_angle[0] = Temp_A;
				}
			}
		}
		break;
	case 12://左边界没找到（右环岛）
		if (flag_type_L == 10)
		{
			flag_type_round = 14;
		}
		else
		{
			Get_angle_L(1, L_edge_Num);
			if (L_edge_Num > 6)
			{
				Temp_A = 0;
				L_edge_angle[0] = 0;
				for (uint8 i = 4; i < L_edge_Num - 2; i++)
				{
					if (L_edge_angle[Temp_A] < L_edge_angle[i])
					{
						Temp_A = i;
					}
				}
				if (L_edge_angle[Temp_A] > angle_is_S)
				{
					L_edge_angle[0] = Temp_A;
				}
			}
		}
		break;
	case 13://直道边找到了（左环岛）
		if (flag_type_R != 10 && R_edge_Y[1] < Num_lim_search_UP * 3)
		{
			flag_type_round = 15;
		}
		break;
	case 14://直道边找到了（右环岛）
		if (flag_type_L != 10 && L_edge_Y[1] < Num_lim_search_UP * 3)
		{
			flag_type_round = 16;
		}
		break;
	case 15:
		if (flag_type_L == 10)
		{
			IF_Warning_MOD = 3;
		}
		break;
	case 16:
		if (flag_type_R == 10)
		{
			IF_Warning_MOD = 3;
		}
		break;
	case 17:
		//环岛结束（左环岛）
		IF_Warning_MOD = 3;
		flag_type_round = 15;
		if (flag_type_R < 4 && flag_type_L < 4)
		{
			IF_Warning_MOD = 0;
			flag_type = 1;
			flag_type_curve = 0;
			flag_type_cross = 0;
			flag_type_round = 0;
		}
		break;
	case 18:
		//环岛结束（右环岛）
		IF_Warning_MOD = 3;
		flag_type_round = 16;
		if (flag_type_R < 4 && flag_type_L < 4)
		{
			IF_Warning_MOD = 0;
			flag_type = 1;
			flag_type_curve = 0;
			flag_type_cross = 0;
			flag_type_round = 0;
		}
		break;
	}
	//对不同阶段的环岛进行处理
	switch (flag_type_round)
	{
	case 0://仅有锐角拐点且一边为直线
		break;
	case 1://一边直线，另一个有锐角拐点，向上能找到环物
		Croos_turn_point_Num[1] = Find_MAX_Y(1, 1, L_edge_Num);
		Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
		Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
		//删除错误点
		Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
		//向上补线
		//左边沿向上扫描处理
		Croos_Search_up_L2();
		//无上方转折点，计算斜率，向上拟定边界****************************************************************需要改动
		if (Cross_L_IF_2)
		{
			if (L_edge_Y[0] < IMG_H - Num_lim_search_UP)
			{
				Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
				}
				else
				{
					//删除点（第二段只剩最后一点）
					Del_Data_from_A_to_B_L(Croos_turn_point_Num[1] + 1, L_edge_Num - 1);
					Croos_turn_point_Num[3] = Croos_turn_point_Num[1] + 1;
				}
			}
			else
			{
				IF_Warning_MOD = 1;
			}
			if (IF_Warning_MOD)
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(1);
				IF_Warning_MOD = 0;
			}
			else
			{
				TWO_Point_Get_edge_L(L_edge_X[L_edge_Num - 1], L_edge_Y[L_edge_Num - 1], L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num], L_edge_Num - 1);
			}
		}
		else
		{
			//无上方转折点，计算斜率，向上拟定边界
			Get_K_To_get_UP_edge(1);
			flag_type_round = 0;
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 2://一边直线，另一个有锐角拐点，向上能找到环物
		Croos_turn_point_Num[2] = Find_MAX_Y(0, 1, R_edge_Num);
		Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
		Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
		//删除错误点
		Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
		//向上补线
		//右边沿向上扫描处理
		Croos_Search_up_R2();
		if (Cross_R_IF_2)
		{
			if (R_edge_Y[0] < IMG_H - Num_lim_search_UP)
			{
				Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
				}
				else
				{
					//删除点（第二段只剩最后一点）
					Del_Data_from_A_to_B_R(Croos_turn_point_Num[2] + 1, R_edge_Num - 1);
					Croos_turn_point_Num[4] = Croos_turn_point_Num[2] + 1;
				}
			}
			else
			{
				IF_Warning_MOD = 1;
			}
			if (IF_Warning_MOD)
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(0);
				IF_Warning_MOD = 0;
			}
			else
			{
				TWO_Point_Get_edge_R(R_edge_X[R_edge_Num - 1], R_edge_Y[R_edge_Num - 1], R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num], R_edge_Num - 1);
			}
		}
		else
		{
			//无上方转折点，计算斜率，向上拟定边界
			Get_K_To_get_UP_edge(0);
			flag_type_round = 0;
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 3://一边没找到起始点
		Temp_B = R_edge_Y[R_edge_Num];
		Cross_L_IF_2 = 0;
		while (Temp_B > IMG_H - 25 && !Cross_L_IF_2)
		{
			Find_L_UP_point(R_edge_X[R_edge_Num], Temp_B);
			Temp_B--;
		}
		if (Cross_L_IF_2)
		{
			Temp_A = (R_edge_X[R_edge_Num] + L_edge_X[1]) / 2;
			Temp_B = R_edge_X[1] - Temp_A;
			if (Temp_A - Temp_B <= 0 || Temp_A - Temp_B >= IMG_W)
			{
				Temp_B = Temp_A;
			}
			TWO_Point_Get_edge_L(Temp_A - Temp_B, 1, L_edge_X[1], L_edge_Y[1], 1);
			Mid_Num = IMG_H;
			for (uint8 i = 1; i <= Mid_Num; i++)
			{
				Mid_X[i] = Temp_A;
				Mid_Y[i] = i - 1;
			}
		}
		else
		{
			Mid_Num = IMG_H;
			for (uint8 i = 1; i <= Mid_Num; i++)
			{
				Mid_X[i] = IMG_MID;
				Mid_Y[i] = i - 1;
			}
		}
		IF_Warning_MOD = 2;
		break;
	case 4://一边没找到起始点
		Temp_B = L_edge_Y[L_edge_Num];
		Cross_R_IF_2 = 0;
		while (Temp_B > IMG_H - 25 && !Cross_R_IF_2)
		{
			Find_R_UP_point(L_edge_X[L_edge_Num], Temp_B);
			Temp_B--;
		}
		if (Cross_R_IF_2)
		{
			Temp_A = (R_edge_X[1] + L_edge_X[L_edge_Num]) / 2;
			Temp_B = Temp_A - L_edge_X[1];
			if (Temp_A + Temp_B <= 0 || Temp_A + Temp_B >= IMG_W)
			{
				Temp_B = IMG_W - 1 - Temp_A;
			}
			TWO_Point_Get_edge_R(Temp_A + Temp_B, 1, R_edge_X[1], R_edge_Y[1], 1);
			Mid_Num = IMG_H;
			for (uint8 i = 1; i <= Mid_Num; i++)
			{
				Mid_X[i] = Temp_A;
				Mid_Y[i] = i - 1;
			}
		}
		else
		{
			Mid_Num = IMG_H;
			for (uint8 i = 1; i <= Mid_Num; i++)
			{
				Mid_X[i] = IMG_MID;
				Mid_Y[i] = i - 1;
			}
		}
		IF_Warning_MOD = 2;
		break;
	case 5://一边起始点为环物*******************************************************************************************去除独立
		if (flag_type_L < 4)
		{
			//左赛道从上方出去
		}
		else if (flag_type_L == 4 || flag_type_L == 5 || flag_type_L == 6 || flag_type_L == 10)
		{
			if (flag_type_L == 10)
			{
				Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
			}
			//左赛道从左方出去
			//如果右边没找到
			if (flag_type_R == 10)
			{
				Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
				flag_type_R = 1;
				//先看一下上方有没有点
				Find_L_UP_point_W(IMG_W - Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
				Find_L_UP_point(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
				Del_Data_N_L(L_edge_Num - 1);
			}
			else
			{
				//先看一下上方有没有点
				Find_L_UP_point(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
			}
			if (Cross_L_IF_2)//点是否独立
			{
				for (uint8 i = 1; i < L_edge_Num; i++)
				{
					if (L_edge_X[i] == L_edge_X[L_edge_Num] && L_edge_Y[i] == L_edge_Y[L_edge_Num])
					{
						Del_Data_Now_L();
						Cross_L_IF_2 = 0;
						break;
					}
				}
			}
			if (Cross_L_IF_2)//有独立点
			{
				//右边沿清空（只剩一点）
				Del_Data_from_N_R(2);
				//将独立点复制到右边沿
				Put_Data_to_R_edge(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
				//删除独立点，从左边沿的数组中
				Del_Data_Now_L();
				//进行search8
				IF_Search_8_Cross_SP = 1;
				Search_8_R(R_edge_X[2], R_edge_Y[2]);
				//寻找最低点
				Temp_B = 255;
				Temp_A = 2;
				for (uint8 i = 2; i <= R_edge_Num; i++)
				{
					if (R_edge_Y[i] < Temp_B)
					{
						Temp_B = R_edge_Y[i];
						Temp_A = i;
					}
				}
				//删除多余点
				if (Temp_A != 2)
				{
					Del_Data_from_A_to_B_R(2, Temp_A - 1);
				}
				//连接点
				TWO_Point_Get_edge_R(R_edge_X[1], R_edge_Y[1], R_edge_X[2], R_edge_Y[2], 2);
			}
			else//无点
			{
				Temp_B = R_edge_X[R_edge_Num] - Num_lim_search_LR;
				Temp_A = R_edge_Num;
				for (uint8 i = 1; i <= Temp_B; i++)
				{
					Put_Data_to_R_edge(R_edge_X[Temp_A] - i, IMG_H - Num_lim_search_UP);
				}
			}
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 6://一边起始点为环物
		if (flag_type_R < 4)
		{
			//右赛道从上方出去
		}
		else if (flag_type_R == 7 || flag_type_R == 8 || flag_type_R == 9 || flag_type_R == 10)
		{
			if (flag_type_R == 10)
			{
				Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
			}
			//右赛道从右方出去
			//如果左边没找到
			if (flag_type_L == 10)
			{
				Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
				flag_type_L = 1;
				//先看一下上方有没有点
				Find_R_UP_point_W(Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
				Find_R_UP_point(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
				Del_Data_N_R(R_edge_Num - 1);
			}
			else
			{
				//先看一下上方有没有点
				Find_R_UP_point(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
			}
			if (Cross_R_IF_2)//点是否独立
			{
				for (uint8 i = 1; i < R_edge_Num; i++)
				{
					if (R_edge_X[i] == R_edge_X[R_edge_Num] && R_edge_Y[i] == R_edge_Y[R_edge_Num])
					{
						Del_Data_Now_R();
						Cross_R_IF_2 = 0;
						break;
					}
				}
			}
			if (Cross_R_IF_2)//有独立点
			{
				//左边沿清空（只剩一点）
				Del_Data_from_N_L(2);
				//将独立点复制到左边沿
				Put_Data_to_L_edge(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
				//删除独立点，从右边沿的数组中
				Del_Data_Now_R();
				//进行search8
				IF_Search_8_Cross_SP = 1;
				Search_8_L(L_edge_X[2], L_edge_Y[2]);
				//寻找最低点
				Temp_B = 255;
				Temp_A = 2;
				for (uint8 i = 2; i <= L_edge_Num; i++)
				{
					if (L_edge_Y[i] < Temp_B)
					{
						Temp_B = L_edge_Y[i];
						Temp_A = i;
					}
				}
				//删除多余点
				if (Temp_A != 2)
				{
					Del_Data_from_A_to_B_L(2, Temp_A - 1);
				}
				//连接点
				TWO_Point_Get_edge_L(L_edge_X[1], L_edge_Y[1], L_edge_X[2], L_edge_Y[2], 2);
			}
			else//无点
			{
				Temp_B = IMG_W - L_edge_X[L_edge_Num] - Num_lim_search_LR;
				Temp_A = L_edge_Num;
				for (uint8 i = 1; i <= Temp_B; i++)
				{
					Put_Data_to_L_edge(L_edge_X[Temp_A] + i, IMG_H - Num_lim_search_UP);
				}
			}
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 7://直道没边
		Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
		flag_type_R = 1;
		//寻找上方点
		Find_L_UP_point(IMG_W - Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
		if (Cross_L_IF_2)//有点
		{
			//右边沿清空（只剩一点）
			Del_Data_from_N_R(2);
			//将独立点复制到右边沿
			Put_Data_to_R_edge(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
			//删除独立点，从左边沿的数组中
			Del_Data_Now_L();
			//进行search8
			IF_Search_8_Cross_SP = 1;
			Search_8_R(R_edge_X[2], R_edge_Y[2]);
			//寻找最低点
			Temp_B = 255;
			Temp_A = 2;
			for (uint8 i = 2; i <= R_edge_Num; i++)
			{
				if (R_edge_Y[i] < Temp_B)
				{
					Temp_B = R_edge_Y[i];
					Temp_A = i;
				}
			}
			//删除多余点
			if (Temp_A != 2)
			{
				Del_Data_from_A_to_B_R(2, Temp_A - 1);
			}
			//连接点
			TWO_Point_Get_edge_R(R_edge_X[1], R_edge_Y[1], R_edge_X[2], R_edge_Y[2], 2);
		}
		else//无点
		{
			God_FUCK_me(0);
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 8://直道没边
		Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
		flag_type_L = 1;
		//寻找上方点
		Find_R_UP_point(Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
		if (Cross_R_IF_2)//有点
		{
			//左边沿清空（只剩一点）
			Del_Data_from_N_L(2);
			//将独立点复制到左边沿
			Put_Data_to_L_edge(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
			//删除独立点，从右边沿的数组中
			Del_Data_Now_R();
			//进行search8
			IF_Search_8_Cross_SP = 1;
			Search_8_L(L_edge_X[2], L_edge_Y[2]);
			//寻找最低点
			Temp_B = 255;
			Temp_A = 2;
			for (uint8 i = 2; i <= L_edge_Num; i++)
			{
				if (L_edge_Y[i] < Temp_B)
				{
					Temp_B = L_edge_Y[i];
					Temp_A = i;
				}
			}
			//删除多余点
			if (Temp_A != 2)
			{
				Del_Data_from_A_to_B_L(2, Temp_A - 1);
			}
			//连接点
			TWO_Point_Get_edge_L(L_edge_X[1], L_edge_Y[1], L_edge_X[2], L_edge_Y[2], 2);
		}
		else//无点
		{
			God_FUCK_me(1);
		}
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 9://弯道0
		break;
	case 10://弯道
		break;
	case 11://右边出现锐角（左环岛）
		if (flag_type_L == 10)
		{
			Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
		}
		Del_Data_from_N_R(R_edge_angle[0] + 1);
		TWO_Point_Get_edge_R(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num], Num_lim_search_LR, IMG_H - Num_lim_search_UP, R_edge_Num + 1);
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 12://左边出现锐角（右环岛）
		if (flag_type_R == 10)
		{
			Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
		}
		Del_Data_from_N_L(L_edge_angle[0] + 1);
		TWO_Point_Get_edge_L(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num], IMG_W - Num_lim_search_LR, IMG_H - Num_lim_search_UP, L_edge_Num + 1);
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 13://右边界没找到（左环岛）
		if (flag_type_L == 10)
		{
			Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
			flag_type_L = 6;
		}
		Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
		TWO_Point_Get_edge_R(R_edge_X[1], R_edge_Y[1], Num_lim_search_LR, IMG_H - Num_lim_search_UP, 2);
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 14://左边界没找到（右环岛）
		if (flag_type_R == 10)
		{
			Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
			flag_type_R = 9;
		}
		Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
		TWO_Point_Get_edge_L(L_edge_X[1], L_edge_Y[1], IMG_W - Num_lim_search_LR, IMG_H - Num_lim_search_UP, 2);
		Mid_Get();
		IF_Warning_MOD = 2;
		break;
	case 15://直道边找到了（左环岛）
		if (IF_Warning_MOD == 3)
		{
			flag_type_round = 17;
		}
		if (flag_type_L < 4)
		{
			//左赛道从上方出去
			Mid_Get();
			IF_Warning_MOD = 2;
		}
		else if (flag_type_L == 4 || flag_type_L == 5 || flag_type_L == 6)
		{
			//左赛道从左方出去
			//如果右边没找到
			if (flag_type_R == 10)
			{
				God_FUCK_me(0);
				flag_type_R = 1;
				//先看一下上方有没有点
				Find_L_UP_point_W(IMG_W - Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
				Find_L_UP_point(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
				Del_Data_N_L(L_edge_Num - 1);
			}
			else
			{
				//先看一下上方有没有点
				Find_L_UP_point(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
			}
			if (Cross_L_IF_2)//点是否独立
			{
				for (uint8 i = 1; i < L_edge_Num; i++)
				{
					if (L_edge_X[i] == L_edge_X[L_edge_Num] && L_edge_Y[i] == L_edge_Y[L_edge_Num])
					{
						Del_Data_Now_L();
						Cross_L_IF_2 = 0;
						break;
					}
				}
			}
			if (Cross_L_IF_2)//有独立点
			{
				if (flag_type_L == 10)
				{
					flag_type_round = 17;
					Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
					flag_type_L = 6;
				}
				//删除除独立点和起始点之间的数据
				Del_Data_from_A_to_B_L(2, L_edge_Num - 1);
				//连接点
				TWO_Point_Get_edge_L(L_edge_X[1], L_edge_Y[1], L_edge_X[2], L_edge_Y[2], 2);
				Mid_Get();
			}
			else//无点
			{
				Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
				//Mid_Num = IMG_H;
				//for (uint8 i = 1; i <= IMG_H; i++)
				//{
				//	Mid_X[i] = IMG_mid_L;
				//	Mid_Y[i] = i;
				//}
			}
			IF_Warning_MOD = 2;
		}
		else if (flag_type_L == 10)
		{
			Temp_B = R_edge_Y[R_edge_Num];
			Cross_L_IF_2 = 0;
			while (Temp_B > IMG_H - 25 && !Cross_L_IF_2)
			{
				Find_L_UP_point(R_edge_X[R_edge_Num], Temp_B);
				Temp_B--;
			}
			if (Cross_L_IF_2)
			{
				Temp_A = (R_edge_X[R_edge_Num] + L_edge_X[1]) / 2;
				Temp_B = R_edge_X[1] - Temp_A;
				if (Temp_A - Temp_B <= 0 || Temp_A - Temp_B >= IMG_W)
				{
					Temp_B = Temp_A;
				}
				TWO_Point_Get_edge_L(Temp_A - Temp_B, 1, L_edge_X[1], L_edge_Y[1], 1);
				Mid_Num = IMG_H;
				for (uint8 i = 1; i <= Mid_Num; i++)
				{
					Mid_X[i] = Temp_A;
					Mid_Y[i] = i - 1;
				}
			}
			else
			{
				Put_Data_to_L_edge(Num_lim_search_LR, Num_lim_search_UP);
				//Mid_Num = IMG_H;
				//for (uint8 i = 1; i <= Mid_Num; i++)
				//{
				//	Mid_X[i] = IMG_MID;
				//	Mid_Y[i] = i - 1;
				//}
			}
			IF_Warning_MOD = 2;
		}
		break;
	case 16://直道边找到了（右环岛）
		if (IF_Warning_MOD == 3)
		{
			flag_type_round = 18;
		}
		if (flag_type_R < 4)
		{
			//右赛道从上方出去
			Mid_Get();
			IF_Warning_MOD = 2;
		}
		else if (flag_type_R == 7 || flag_type_R == 8 || flag_type_R == 9)
		{
			//右赛道从右方出去
			//如果左边没找到
			if (flag_type_L == 10)
			{
				God_FUCK_me(1);
				flag_type_L = 1;
				//先看一下上方有没有点
				Find_R_UP_point_W(Num_lim_search_LR, IMG_H - 1 - Num_lim_search_UP);
				Find_R_UP_point(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
				Del_Data_N_R(R_edge_Num - 1);
			}
			else
			{
				//先看一下上方有没有点
				Find_R_UP_point(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
			}
			if (Cross_R_IF_2)//点是否独立
			{
				for (uint8 i = 1; i < R_edge_Num; i++)
				{
					if (R_edge_X[i] == R_edge_X[R_edge_Num] && R_edge_Y[i] == R_edge_Y[R_edge_Num])
					{
						Del_Data_Now_R();
						Cross_R_IF_2 = 0;
						break;
					}
				}
			}
			if (Cross_R_IF_2)//有独立点
			{
				if (flag_type_R == 10)
				{
					flag_type_round = 18;
					Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
					flag_type_R = 9;
				}
				//删除除独立点和起始点之间的数据
				Del_Data_from_A_to_B_R(2, R_edge_Num - 1);
				//连接点
				TWO_Point_Get_edge_R(R_edge_X[1], R_edge_Y[1], R_edge_X[2], R_edge_Y[2], 2);
				Mid_Get();
			}
			else//无点
			{
				Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
				//Mid_Num = IMG_H;
				//for (uint8 i = 1; i <= IMG_H; i++)
				//{
				//	Mid_X[i] = IMG_mid_L;
				//	Mid_Y[i] = i;
				//}
			}
			IF_Warning_MOD = 2;
		}
		else if (flag_type_R == 10)
		{
			Temp_B = L_edge_Y[L_edge_Num];
			Cross_R_IF_2 = 0;
			while (Temp_B > IMG_H - 25 && !Cross_R_IF_2)
			{
				Find_R_UP_point(L_edge_X[L_edge_Num], Temp_B);
				Temp_B--;
			}
			if (Cross_R_IF_2)
			{
				Temp_A = (R_edge_X[1] + L_edge_X[L_edge_Num]) / 2;
				Temp_B = Temp_A - L_edge_X[1];
				if (Temp_A + Temp_B <= 0 || Temp_A + Temp_B >= IMG_W)
				{
					Temp_B = IMG_W - 1 - Temp_A;
				}
				TWO_Point_Get_edge_R(Temp_A + Temp_B, 1, R_edge_X[1], R_edge_Y[1], 1);
				Mid_Num = IMG_H;
				for (uint8 i = 1; i <= Mid_Num; i++)
				{
					Mid_X[i] = Temp_A;
					Mid_Y[i] = i - 1;
				}
			}
			else
			{
				Put_Data_to_R_edge(IMG_W - Num_lim_search_LR, Num_lim_search_UP);
				//Mid_Num = IMG_H;
				//for (uint8 i = 1; i <= Mid_Num; i++)
				//{
				//	Mid_X[i] = IMG_MID;
				//	Mid_Y[i] = i - 1;
				//}
			}
			IF_Warning_MOD = 2;
		}
		break;
	}
}

//岔道核心程序
void Y_main(void)
{
	uint8 Temp;
	uint8 Temp2 = 1;
	if (!L_edge_Num && !R_edge_Num)
	{
		flag_type_Y = 2;
	}
	if (!R_edge_Num)
	{
		R_edge_X[0] = IMG_W - Num_lim_search_LR;
	}
	else
	{
		Temp = Find_MIN_X(0, 1, R_edge_Num);
		R_edge_X[0] = R_edge_X[Temp] - Num_lim_search_LR;
		Del_Data_from_N_R(1);
	}
	if (!L_edge_Num)
	{
		L_edge_X[0] = Num_lim_search_LR;
	}
	else
	{
		Temp = Find_MAX_X(1, 1, L_edge_Num);
		L_edge_X[0] = L_edge_X[Temp] + Num_lim_search_LR;
		Del_Data_from_N_L(1);
	}
	for (uint8 i = L_edge_X[0] + 1; i < R_edge_X[0]; i++)
	{
		for (uint8 j = Num_lim_search_UP; j < IMG_H; j++)
		{
			if (img[j][i] == Black)
			{
				Put_Data_to_L_edge(i, j);
				break;
			}
		}
	}
	Temp = 255;
	for (uint8 i = 1; i <= L_edge_Num; i++)
	{
		if (L_edge_Y[i] < Temp)
		{
			if (L_edge_X[i] > 10 && L_edge_X[i] < IMG_W - 10)
			{
				Temp = L_edge_Y[i];
				Temp2 = i;
			}
		}
	}
	Mid_Num = Temp + 1;
	for (uint8 i = 1; i <= Mid_Num; i++)
	{
		Mid_X[i] = L_edge_X[Temp2];
		Mid_Y[i] = i - 1;
	}
}
//岔道核心2
void Y_main_2(void)
{
	uint8 Temp;
	uint8 Temp2 = 0;
	R_edge_X[0] = IMG_W - Num_lim_search_LR;
	L_edge_X[0] = Num_lim_search_LR;
	for (uint8 i = L_edge_X[0] + 1; i < R_edge_X[0]; i++)
	{
		for (uint8 j = 0; j < IMG_H; j++)
		{
			if (img[j][i] == Black)
			{
				Put_Data_to_L_edge(i, j);
				if (!j)
				{
					Temp2 = 1;
				}
				break;
			}
		}
		if (Temp2)
		{
			break;
		}
	}
	if (Temp2)
	{
		//for (uint8 i = 0; i < IMG_W; i++)
		//{
		//	if (img[0][i] == Black)
		//	{
		//		Put_Data_to_R_edge(i, 0);
		//	}
		//}
		//if (R_edge_Num > 80)
		//{
			Mid_Num = 1;
			Mid_X[1] = R_edge_X[R_edge_Num / 2];
			Mid_Y[1] = 0;
			flag_type = 4;
			flag_type_Y = 3;
		//}
		//else
		//{
		//	Mid_Num = 1;
		//	Mid_X[1] = R_edge_X[(R_edge_Num / 2) + 1];
		//	Mid_Y[1] = 0;
		//}
	}
	else
	{
		Temp = 255;
		for (uint8 i = 1; i <= L_edge_Num; i++)
		{
			if (L_edge_Y[i] < Temp)
			{
				Temp = L_edge_Y[i];
				Temp2 = i;
			}
		}
		Mid_Num = Temp + 1;
		for (uint8 i = 1; i <= Mid_Num; i++)
		{
			Mid_X[i] = L_edge_X[Temp2];
			Mid_Y[i] = i - 1;
		}
	}
}

void Print_for_debug(void)
{
	Print_Mid_and_Edge();
	for (int i = 0; i < IMG_H; i++)
	{
		for (int j = 0; j < IMG_W; j++)
		{
			printf("%d ", img[i][j]);
		}
		printf("\n");
	}
}

//主程序
uint8 Search_main(void)
{
	uint8 flag_type_Number_judge;//flag_type_Number常用的一个flag
	Init();
	Y_change();

	//岔道执行
	if (flag_type == 4 && flag_type_Y == 2 && IF_Y)
	{
		Y_main_2();
		Print_Mid_and_Edge();
		return 1;
	}
	if (flag_type == 4 && flag_type_Y == 3 && IF_Y)
	{
		return 1;
	}
	////岔路再判断
	//if (How_many_White_route(IMG_H / 2) == 30)
	//{
	//	if (flag_type == 5 || flag_type_round)
	//	{

	//	}
	//	else
	//	{
	//		flag_type = 4;
	//		flag_type_Y = 2;
	//		return 1;
	//	}
	//}
	//第一步，寻找边沿开始的点
	Search_start_Y_L = Num_up_search_skip;
	Search_start_Y_R = Num_up_search_skip;
	flag_Search_8_RL = 1;
	Search_for_L_start_point(1);
	if (IF_Warning_MOD)
	{
		return 0;
	}
	if (flag_temp_ != 1)
	{
		flag_temp_ = 0;
		flag_Search_8_RL = 2;
		Search_for_R_start_point(1);
		if (IF_Warning_MOD)
		{
			return 0;
		}
	}

	//第二步，在找到起始点的条件下开始边界扫描
	//左边沿扫描
	if (flag_type_L != 10)
	{
		//初始化，左边沿
		Search_8_mid_Num_Will = Search_8_mid_Num_Will_rem[1];
		IF_Search_8_Continue = 1;
		flag_Search_8_RL = 1;
		edge_X = L_edge_X[L_edge_Num];
		edge_Y = L_edge_Y[L_edge_Num];
		//进行扫描，左边沿
		while (IF_Search_8_Continue)
		{
			Search_8();
			if (IF_Warning_MOD)
			{
				if (flag_type == 5)
				{
					break;
				}
				else
				{
					return 0;
				}
			}
			Judge();
		}
		if (flag_type_L == 10 && L_edge_Num > 0)
		{
			flag_type_L = 5;
		}
	}

	//右边沿扫描
	if (flag_type_R != 10)
	{
		//初始化，右边沿
		Search_8_mid_Num_Will = Search_8_mid_Num_Will_rem[2];
		IF_Search_8_Continue = 1;
		flag_Search_8_RL = 2;
		edge_X = R_edge_X[R_edge_Num];
		edge_Y = R_edge_Y[R_edge_Num];
		//进行扫描，右边沿
		while (IF_Search_8_Continue)
		{
			Search_8();
			if (IF_Warning_MOD)
			{
				if (flag_type == 5)
				{
					break;
				}
				else
				{
					return 0;
				}
			}
			Judge();
		}
		if (flag_type_R == 10 && R_edge_Num > 0)
		{
			flag_type_R = 8;
		}
	}

	//第三步，赛道类型判断，并寻找错误边沿和需要补全的边沿
	flag_type_Number = (flag_type_L - 1) * 10 + (flag_type_R - 1);//将左道10种情况和右道10种情况汇总为100种情况

	//岔路判断
	if (flag_type_Y || flag_type == 4)
	{
		if (IF_Y)
		{
			flag_type = 4;
			if (!flag_type_Y)
			{
				flag_type_Y = 1;
			}
			Y_main();
			Print_Mid_and_Edge();
			return 1;
		}
	}

	//上一个图像是否为环岛
	if (flag_type == 5 || flag_type_round)
	{
		flag_type = 5;
		Round_main();
	}
	if (IF_Warning_MOD)
	{
		//绘画中线和边界
		Print_Mid_and_Edge();
		IF_Warning_MOD = 0;
		return 1;
	}

	//进行角度计算
	Get_angle_L(1, L_edge_Num);
	Get_angle_R(1, R_edge_Num);
	flag_type_Number_judge = 0;
	if (L_edge_Num > 6)
	{
		for (uint8 i = 4; i < L_edge_Num - 2; i++)
		{
			if (L_edge_angle[i] > angle_is_S)
			{
				flag_type_Number_judge++;
				break;
			}
		}
	}
	if (R_edge_Num > 6)
	{
		for (uint8 i = 4; i < R_edge_Num - 2; i++)
		{
			if (R_edge_angle[i] > angle_is_S)
			{
				flag_type_Number_judge += 2;
				break;
			}
		}
	}
	if (flag_type_Number_judge)
	{
		switch (flag_type_Number_judge)
		{
		case 1:
			if (!IF_Many_edge_Other_Way_L())
			{
				if (IF_Straight_R())
				{
					flag_type_Number = 103;
					flag_type = 5;
					flag_type_curve = 0;
					flag_type_cross = 0;
					flag_type_round = 1;
				}
				else
				{
					if (L_edge_Num < CrossEdgeNumMustGreaterThen && R_edge_Num < CrossEdgeNumMustGreaterThen)
					{
						if (R_edge_Num < 5)
						{
							Del_Data_from_N_R(1);
							flag_type_R = 10;
						}
						else
						{
							flag_type_Number = 47;
						}
					}
				}
			}
			break;
		case 2:
			if (!IF_Many_edge_Other_Way_R())
			{
				if (IF_Straight_L())
				{
					flag_type_Number = 104;
					flag_type = 5;
					flag_type_curve = 0;
					flag_type_cross = 0;

					flag_type_round = 2;
				}
				else
				{
					if (L_edge_Num < CrossEdgeNumMustGreaterThen && R_edge_Num < CrossEdgeNumMustGreaterThen)
					{
						if (L_edge_Num < 5)
						{
							Del_Data_from_N_L(1);
							flag_type_L = 10;
						}
						else
						{
							flag_type_Number = 47;
						}
					}
				}
			}
			break;
		case 3:
			flag_type_Number = 47;
			break;
		}
		if (flag_type_L == 10)
		{
			if (!IF_Many_edge_Other_Way_R() && R_edge_Num < 100)
			{
				flag_type_Number = 101;
				flag_type = 3;
				flag_type_curve = 0;
				flag_type_cross = 5;

			}
		}
		if (flag_type_R == 10)
		{
			if (!IF_Many_edge_Other_Way_L() && L_edge_Num < 100)
			{
				flag_type_Number = 102;
				flag_type = 3;
				flag_type_curve = 0;
				flag_type_cross = 4;

			}
		}
	}

	//错误情况：左边没边，但是右边是右拐
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_No_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_No_L[i])
		{
			flag_type_Number_judge = 1;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type_L = 1;
		//第一步，寻找边沿开始的点
		Search_start_Y_L = Num_up_search_skip;
		flag_Search_8_RL = 1;
		Search_for_L_start_point(0);
		if (IF_Warning_MOD)
		{
			return 0;
		}
		//第二步，左边沿扫描
		if (flag_type_L != 10)
		{
			//初始化，左边沿
			Search_8_mid_Num_Will = Search_8_mid_Num_Will_rem[1];
			IF_Search_8_Continue = 1;
			flag_Search_8_RL = 1;
			edge_X = L_edge_X[L_edge_Num];
			edge_Y = L_edge_Y[L_edge_Num];
			//进行扫描，左边沿
			while (IF_Search_8_Continue)
			{
				Search_8();
				if (IF_Warning_MOD)
				{
					return 0;
				}
				Judge();
			}
			flag_type_Number = 100;
		}
		else
		{
			flag_type_L = How_many_White_route(46);
			if (flag_type_L < 2)
			{
				God_FUCK_me(1);
			}
			else if (flag_type_L == 10 || R_edge_Y[1] < Num_lim_search_UP*2)
			{
				Get_angle_R(1, R_edge_Num);
				R_edge_angle[0] = 0;
				if (R_edge_Num > 6)
				{
					flag_type_Number_judge = 0;
					for (uint8 i = 4; i < R_edge_Num - 2; i++)
					{
						if (R_edge_angle[flag_type_Number_judge] < R_edge_angle[i])
						{
							flag_type_Number_judge = i;
						}
					}
					if (R_edge_angle[flag_type_Number_judge] > angle_is_S)
					{
						R_edge_angle[0] = flag_type_Number_judge;
					}
				}
				if (R_edge_angle[0])
				{
					if (img[IMG_H - 1][R_edge_X[R_edge_angle[0]]])
					{
						flag_type_L = 10;
						flag_type_R = 2;
						flag_type_Number = (flag_type_L - 1) * 10 + (flag_type_R - 1);//将左道10种情况和右道10种情况汇总为100种情况
					}
					else
					{
						//修改flag
						flag_type_Number = 101;
						flag_type = 3;
						flag_type_curve = 0;
						flag_type_cross = 5;
					}
				}
				else
				{
					flag_type_L = 10;
					flag_type_R = 2;
					flag_type_Number = (flag_type_L - 1) * 10 + (flag_type_R - 1);//将左道10种情况和右道10种情况汇总为100种情况
				}
			}
			else
			{
				//修改flag
				flag_type_Number = 101;
				flag_type = 3;
				flag_type_curve = 0;
				flag_type_cross = 5;
			}
			flag_type_L = 10;
		}
	}

	//错误情况：右边没边，但是左边是左拐
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_No_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_No_R[i])
		{
			flag_type_Number_judge = 1;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type_R = 1;
		//第一步，寻找边沿开始的点
		Search_start_Y_R = Num_up_search_skip;
		flag_Search_8_RL = 2;
		Search_for_R_start_point(0);
		if (IF_Warning_MOD)
		{
			return 0;
		}
		//第二步，右边沿扫描
		if (flag_type_R != 10)
		{
			//初始化，右边沿
			Search_8_mid_Num_Will = Search_8_mid_Num_Will_rem[2];
			IF_Search_8_Continue = 1;
			flag_Search_8_RL = 2;
			edge_X = R_edge_X[R_edge_Num];
			edge_Y = R_edge_Y[R_edge_Num];
			//进行扫描，右边沿
			while (IF_Search_8_Continue)
			{
				Search_8();
				if (IF_Warning_MOD)
				{
					return 0;
				}
				Judge();
			}
			flag_type_Number = 100;
		}
		else
		{
			flag_type_R = How_many_White_route(46);
			if (flag_type_R < 2)
			{
				God_FUCK_me(0);
			}
			else if (flag_type_R == 10 || L_edge_Y[1] < Num_lim_search_UP * 2)
			{
				Get_angle_L(1, L_edge_Num);
				L_edge_angle[0] = 0;
				if (L_edge_Num > 6)
				{
					flag_type_Number_judge = 0;
					for (uint8 i = 4; i < L_edge_Num - 2; i++)
					{
						if (L_edge_angle[flag_type_Number_judge] < L_edge_angle[i])
						{
							flag_type_Number_judge = i;
						}
					}
					if (L_edge_angle[flag_type_Number_judge] > angle_is_S)
					{
						L_edge_angle[0] = flag_type_Number_judge;
					}
				}
				if (L_edge_angle[0])
				{
					if (img[IMG_H - 1][L_edge_X[L_edge_angle[0]]])
					{
						flag_type_R = 10;
						flag_type_L = 3;
						flag_type_Number = (flag_type_L - 1) * 10 + (flag_type_R - 1);//将左道10种情况和右道10种情况汇总为100种情况
					}
					else
					{
						//修改flag
						flag_type_Number = 102;
						flag_type = 3;
						flag_type_curve = 0;
						flag_type_cross = 4;
					}
				}
				else
				{
					flag_type_R = 10;
					flag_type_L = 3;
					flag_type_Number = (flag_type_L - 1) * 10 + (flag_type_R - 1);//将左道10种情况和右道10种情况汇总为100种情况
				}
			}
			else
			{
				//修改flag
				flag_type_Number = 102;
				flag_type = 3;
				flag_type_curve = 0;
				flag_type_cross = 4;
			}
			flag_type_R = 10;
		}
	}

	//1)理想直道
	if (!flag_type_Number)
	{
		flag_type = 1;
		flag_type_curve = 0;
		flag_type_cross = 0;
	}

	//2)类拐弯，有可能是环岛，检测两条边界中有无Black
	//2.1）左拐
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_L[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type = 2;
		flag_type_curve = 1;
		flag_type_cross = 0;
	}
	//左拐，大弯
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_S_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_L_S[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number_judge)
	{
		Put_Data_to_L_edge(1, 1);
		flag_type = 2;
		flag_type_curve = 3;
		flag_type_cross = 0;

	}
	//2.1）右拐
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_R[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type = 2;
		flag_type_curve = 2;
		flag_type_cross = 0;
	}
	//右拐，大弯
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_S_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_R_S[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number_judge)
	{
		Put_Data_to_R_edge(IMG_W - 2, 1);
		flag_type = 2;
		flag_type_curve = 4;
		flag_type_cross = 0;

	}
	//特殊弯道
	//左弯道
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_special_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_L_special[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number == 32 && How_many_Black_route(IMG_H - Num_lim_search_UP) == 10)
	{
		flag_type_Number_judge = 0;
		flag_type_Number = 37;
	}
	if (flag_type_Number == 32 || flag_type_Number == 42 || flag_type_Number == 52)
	{
		if (L_edge_Y[1] <= Num_lim_search_UP + Num_up_search_skip + 1 && R_edge_Y[1] <= Num_lim_search_UP + Num_up_search_skip + 1)
		{
			flag_type_Number_judge = 0;
			flag_type_Number = 37;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type = 2;
		flag_type_curve = 1;
		flag_type_cross = 0;

		for (uint8 i = 1; i <= L_edge_Num; i++)
		{
			if (L_edge_Y[i] >= IMG_H - 3 - Num_lim_search_UP)
			{
				flag_type_Number_judge = 0;
				break;
			}
		}
		if (flag_type_Number_judge)
		{
			if (L_edge_Num < SP_curve_or_Cross)
			{
				if (L_edge_Y[1] > Num_lim_search_UP + Num_up_search_skip + 1 || R_edge_Y[1] > Num_lim_search_UP + Num_up_search_skip + 1)
				{
					flag_type_Number = 37;
					flag_type_Number_judge = 0;
				}
			}
		}
		if (How_many_Black_route(IMG_H - Num_lim_search_UP) == 1 && (How_many_White_route(46) == 1 || How_many_White_route(46) == 10))
		{
			if (X_or_Y_dif_MAX(0, 1, R_edge_Num) && IF_Same_Point(1) && IF_Straight_R())
			{
				//修改flag
				flag_type_Number = 103;
				flag_type = 5;
				flag_type_curve = 0;
				flag_type_cross = 0;
				flag_type_round = 1;
			}
		}
	}
	//右弯道
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_curve_special_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_curve_R_special[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	if (flag_type_Number == 16 && How_many_Black_route(IMG_H - Num_lim_search_UP) == 10)
	{
		flag_type_Number_judge = 0;
		flag_type_Number = 37;
	}
	if (flag_type_Number == 16 || flag_type_Number == 17 || flag_type_Number == 18)
	{
		if (L_edge_Y[1] <= Num_lim_search_UP + Num_up_search_skip + 1 && R_edge_Y[1] <= Num_lim_search_UP + Num_up_search_skip + 1)
		{
			flag_type_Number_judge = 0;
			flag_type_Number = 37;
		}
	}
	if (flag_type_Number_judge)
	{
		flag_type = 2;
		flag_type_curve = 2;
		flag_type_cross = 0;
		for (uint8 i = 1; i <= R_edge_Num; i++)
		{
			if (R_edge_Y[i] >= IMG_H - 3 - Num_lim_search_UP)
			{
				flag_type_Number_judge = 0;
				break;
			}
		}
		if (flag_type_Number_judge)
		{
			if (R_edge_Num < SP_curve_or_Cross)
			{
				if (L_edge_Y[1] > Num_lim_search_UP + Num_up_search_skip + 1 || R_edge_Y[1] > Num_lim_search_UP + Num_up_search_skip + 1)
				{
					flag_type_Number = 37;
					flag_type_Number_judge = 0;
				}
			}
		}
		if (How_many_Black_route(IMG_H - Num_lim_search_UP) == 1 && (How_many_White_route(46) == 1 || How_many_White_route(46) == 10))
		{
			if (X_or_Y_dif_MAX(1,1,L_edge_Num) && IF_Same_Point(0) && IF_Straight_L())
			{
				//修改flag
				flag_type_Number = 104;
				flag_type = 5;
				flag_type_curve = 0;
				flag_type_cross = 0;
				flag_type_round = 2;
			}
		}
	}

	//3）十字
	flag_type_Number_judge = 0;
	for (uint8 i = 0; i < NUM_cross_Mod; i++)
	{
		if (flag_type_Number == flag_type_Number_cross[i])
		{
			flag_type_Number_judge = 1;
			break;
		}
	}
	//判断是否为岔路
	if (flag_type_Number_judge && IF_Y)
	{
		if (How_many_White_route(IMG_H - Num_lim_search_UP) == 20 || How_many_White_route(IMG_H - Num_lim_search_UP) == 30)
		{
			if (How_many_Black_route(IMG_H - Num_lim_search_UP) == 1)
			{
				flag_type = 4;
				flag_type_curve = 0;
				flag_type_cross = 0;
				flag_type_Y = 1;
			}
		}
	}
	if (flag_type_Number_judge)
	{

		//修改flag
		flag_type = 3;
		flag_type_curve = 0;
		flag_type_cross = 1;

		//寻找左边沿拐点
		Croos_turn_point_Num[1] = Find_MAX_X(1, 1, L_edge_Num);
		Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
		Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
		if (Croos_turn_point[2] < Cross_Turn_point_too_LOW && Croos_turn_point_Num[1] < Cross_Turn_point_too_LOW)
		{
			Croos_turn_point_Num[1] = Find_MAX_Y(1, 1, L_edge_Num);
			Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
			Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
		}
		Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);

		//寻找右边沿拐点
		Croos_turn_point_Num[2] = Find_MIN_X(0, 1, R_edge_Num);
		Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
		Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
		if (Croos_turn_point[4] < Cross_Turn_point_too_LOW && Croos_turn_point_Num[2] < Cross_Turn_point_too_LOW)
		{
			Croos_turn_point_Num[2] = Find_MAX_Y(0, 1, R_edge_Num);
			Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
			Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
		}
		Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);

		//3.1）是否为十字第1阶段
		//左边沿向上扫描处理
		Croos_Search_up_L2();

		//右边沿向上扫描处理
		Croos_Search_up_R2();

		if (Cross_L_IF_2 && Cross_R_IF_2)
		{
			flag_type = 3;
			flag_type_curve = 0;
			flag_type_cross = 2;
		}
		if (Cross_L_IF_2)
		{
			if (L_edge_Y[0] < IMG_H - Num_lim_search_UP)
			{
				Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
				}
				else
				{
					//找拐点
					Croos_find_turn_point_U_L(Croos_turn_point_Num[1] + 1);
					Del_Data_from_A_to_B_L(Croos_turn_point_Num[1] + 1, Croos_turn_point_Num[3] - 1);
					Croos_turn_point_Num[3] = Croos_turn_point_Num[1] + 1;
				}
			}
			else
			{
				IF_Warning_MOD = 1;
			}
			if (IF_Warning_MOD)
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(1);
				IF_Warning_MOD = 0;
			}
			else
			{
				TWO_Point_Get_edge_L(Croos_turn_point[1], Croos_turn_point[2], Croos_turn_point[5], Croos_turn_point[6], Croos_turn_point_Num[3]);
			}
		}
		else
		{
			//无上方转折点，计算斜率，向上拟定边界
			Get_K_To_get_UP_edge(1);
		}
		if (Cross_R_IF_2)
		{
			if (R_edge_Y[0] < IMG_H - Num_lim_search_UP)
			{
				Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
				}
				else
				{
					//找拐点
					Croos_find_turn_point_U_R(Croos_turn_point_Num[2] + 1);
					Del_Data_from_A_to_B_R(Croos_turn_point_Num[2] + 1, Croos_turn_point_Num[4] - 1);
					Croos_turn_point_Num[4] = Croos_turn_point_Num[2] + 1;
				}
			}
			else
			{
				IF_Warning_MOD = 1;
			}
			if (IF_Warning_MOD)
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(0);
				IF_Warning_MOD = 0;
			}
			else
			{
				TWO_Point_Get_edge_R(Croos_turn_point[3], Croos_turn_point[4], Croos_turn_point[7], Croos_turn_point[8], Croos_turn_point_Num[4]);
			}
		}
		else
		{
			//无上方转折点，计算斜率，向上拟定边界
			Get_K_To_get_UP_edge(0);
		}
	}
	if (flag_type_Y && IF_Y)
	{
		flag_type = 4;
		flag_type_cross = 0;
	}

	//3.3）十字第3阶段
	if (flag_type_Number == 99)
	{
		if (L_edge_Num)
		{
			Del_Data_from_N_L(1);
		}
		if (R_edge_Num)
		{
			Del_Data_from_N_R(1);
		}
		//修改flag
		flag_type = 3;
		flag_type_curve = 0;
		flag_type_cross = 3;


		if (!How_many_Black_route(IMG_H - Num_lim_search_UP))
		{
			flag_type_Number = 0;
		}

		//左边沿向上扫描处理
		Croos_Search_up_L2();
		while (L_edge_Y[L_edge_Num] >= IMG_H - Num_lim_search_UP && Cross_L_IF_2 && L_edge_Num < 20)
		{
			Put_Data_to_L_edge(L_edge_X[L_edge_Num] + 1, Num_lim_search_UP);
			Croos_Search_up_L2();
			Put_Data_to_L_edge(L_edge_X[L_edge_Num], L_edge_Y[0]);
			Del_Data_N_L(L_edge_Num - 1);
		}
		//右边沿向上扫描处理
		Croos_Search_up_R2();
		while (R_edge_Y[R_edge_Num] >= IMG_H - Num_lim_search_UP && Cross_R_IF_2 && R_edge_Num < 20)
		{
			Put_Data_to_R_edge(R_edge_X[R_edge_Num] - 1, Num_lim_search_UP);
			Croos_Search_up_R2();
			Put_Data_to_R_edge(R_edge_X[R_edge_Num], R_edge_Y[0]);
			Del_Data_N_R(R_edge_Num - 1);
		}

		if (Cross_L_IF_2)
		{
			Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
			if (IF_Warning_MOD)
			{
				return 0;
			}
			//找拐点
			//寻找两边沿的第二拐点
			Croos_find_turn_point_U_L(1);
			//删除错误边沿
			Del_Data_from_A_to_B_L(1, Croos_turn_point_Num[3] - 1);
			Croos_turn_point_Num[3] = 1;
			if (!flag_type_Number)
			{
				Croos_turn_point_Num[1] = Find_MAX_X(1, 1, L_edge_Num);
				Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
				Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
				Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
				Get_K_To_get_UP_edge(1);
			}
			//计算斜率值，向下补充边界
			Get_K_To_get_Down_edge(1);
		}
		else
		{
			God_FUCK_me(1);
		}
		if (Cross_R_IF_2)
		{
			Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
			if (IF_Warning_MOD)
			{
				return 0;
			}
			//找拐点
			//寻找两边沿的第二拐点
			Croos_find_turn_point_U_R(1);
			//删除错误边沿
			Del_Data_from_A_to_B_R(1, Croos_turn_point_Num[4] - 1);
			Croos_turn_point_Num[4] = 1;
			if (!flag_type_Number)
			{
				Croos_turn_point_Num[2] = Find_MIN_X(0, 1, R_edge_Num);
				Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
				Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
				Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
				Get_K_To_get_UP_edge(0);
			}
			//计算斜率值，向下补充边界
			Get_K_To_get_Down_edge(0);
		}
		else
		{
			God_FUCK_me(0);
		}
	}

	//是否为特殊模式的十字
	if (flag_type_Number == 101 || flag_type_Number == 102)
	{
		if (flag_type_cross == 4)
		{
			//寻找左边沿拐点
			Croos_turn_point_Num[1] = Find_MAX_X(1, 1, L_edge_Num);
			Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
			Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
			Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);

			//左边沿向上扫描处理
			Croos_Search_up_L2();
			flag_type_Number_judge = 0;
			while (L_edge_Y[L_edge_Num] >= IMG_H - Num_lim_search_UP && Cross_L_IF_2 && flag_type_Number_judge < 20)
			{
				flag_type_Number_judge++;
				Put_Data_to_L_edge(L_edge_X[L_edge_Num] + 1, Num_lim_search_UP);
				Croos_Search_up_L2();
				Put_Data_to_L_edge(L_edge_X[L_edge_Num], L_edge_Y[0]);
				Del_Data_N_L(L_edge_Num - 1);
			}
			if (Cross_L_IF_2)
			{
				Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					return 0;
				}
				//找拐点
				Croos_find_turn_point_U_L(Croos_turn_point_Num[1] + 1);
				Del_Data_from_A_to_B_L(Croos_turn_point_Num[1] + 1, Croos_turn_point_Num[3] - 1);
				Croos_turn_point_Num[3] = Croos_turn_point_Num[1] + 1;
				TWO_Point_Get_edge_L(Croos_turn_point[1], Croos_turn_point[2], Croos_turn_point[5], Croos_turn_point[6], Croos_turn_point_Num[3]);
			}
			else
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(1);
			}

			//寻找右上点
			for (uint8 i = 0; i < L_edge_Num; i++)
			{
				if (L_edge_Y[L_edge_Num - i] <= IMG_H - 1 - Num_lim_search_LR)
				{
					Find_R_UP_point(L_edge_X[L_edge_Num - i], IMG_H - 1 - Num_lim_search_UP);
					break;
				}
			}
			if (Cross_R_IF_2)
			{
				IF_Search_8_Cross_SP = 1;
				Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num]);
				if (IF_Warning_MOD)
				{
					return 0;
				}
				//将边沿顺序颠倒
				Edge_turn(0);
				//寻找两边沿的第二拐点
				Croos_find_turn_point_U_R(1);
				//删除错误边沿
				Del_Data_from_A_to_B_R(1, Croos_turn_point_Num[4] - 1);
				Croos_turn_point_Num[4] = 1;
				//计算斜率值，向下补充边界
				Get_K_To_get_Down_edge(0);
			}
			else
			{
				God_FUCK_me(0);
			}
		}
		if (flag_type_cross == 5)
		{
			//寻找右边沿拐点
			Croos_turn_point_Num[2] = Find_MIN_X(0, 1, R_edge_Num);
			Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
			Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
			Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);

			//右边沿向上扫描处理
			Croos_Search_up_R2();
			flag_type_Number_judge = 0;
			while (R_edge_Y[R_edge_Num] >= IMG_H - Num_lim_search_UP && Cross_R_IF_2 && flag_type_Number_judge < 20)
			{
				flag_type_Number_judge++;
				Put_Data_to_R_edge(R_edge_X[R_edge_Num] - 1, Num_lim_search_UP);
				Croos_Search_up_R2();
				Put_Data_to_R_edge(R_edge_X[R_edge_Num], R_edge_Y[0]);
				Del_Data_N_R(R_edge_Num - 1);
			}
			if (Cross_R_IF_2)
			{
				Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[0]);
				if (IF_Warning_MOD)
				{
					return 0;
				}
				//找拐点
				Croos_find_turn_point_U_R(Croos_turn_point_Num[2] + 1);
				Del_Data_from_A_to_B_R(Croos_turn_point_Num[2] + 1, Croos_turn_point_Num[4] - 1);
				Croos_turn_point_Num[4] = Croos_turn_point_Num[2] + 1;
				TWO_Point_Get_edge_R(Croos_turn_point[3], Croos_turn_point[4], Croos_turn_point[7], Croos_turn_point[8], Croos_turn_point_Num[4]);
			}
			else
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(0);
			}

			//寻找左上点
			for (uint8 i = 0; i < R_edge_Num; i++)
			{
				if (R_edge_Y[R_edge_Num - i] <= IMG_H - 1 - Num_lim_search_LR)
				{
					Find_L_UP_point(R_edge_X[R_edge_Num - i], IMG_H - 1 - Num_lim_search_UP);
					break;
				}
			}

			if (Cross_L_IF_2)
			{
				IF_Search_8_Cross_SP = 1;
				Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num]);
				if (IF_Warning_MOD)
				{
					return 0;
				}
				//将边沿顺序颠倒
				Edge_turn(1);
				//寻找两边沿的第二拐点
				Croos_find_turn_point_U_L(1);
				//删除错误边沿
				Del_Data_from_A_to_B_L(1, Croos_turn_point_Num[3] - 1);
				Croos_turn_point_Num[3] = 1;
				//计算斜率值，向下补充边界
				Get_K_To_get_Down_edge(1);
			}
			else
			{
				God_FUCK_me(1);
			}
		}
	}

	//5）环岛
	if (flag_type_Number == 103 || flag_type_Number == 104)
	{
		if (flag_type_round == 1)
		{
			//寻找拐点
			Croos_turn_point_Num[1] = Find_MAX_Y(1, 1, L_edge_Num);
			Croos_turn_point[1] = L_edge_X[Croos_turn_point_Num[1]];
			Croos_turn_point[2] = L_edge_Y[Croos_turn_point_Num[1]];
			//删除错误点
			Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
			//向上补线
			//左边沿向上扫描处理
			Croos_Search_up_L2();
			//无上方转折点，计算斜率，向上拟定边界****************************************************************需要改动
			if (Cross_L_IF_2)
			{
				if (L_edge_Y[0] < IMG_H - Num_lim_search_UP)
				{
					Search_8_L(L_edge_X[L_edge_Num], L_edge_Y[0]);
					if (IF_Warning_MOD)
					{
						Del_Data_from_N_L(Croos_turn_point_Num[1] + 1);
					}
					else
					{
						//删除点（第二段只剩最后一点）
						Del_Data_from_A_to_B_L(Croos_turn_point_Num[1] + 1, L_edge_Num - 1);
						Croos_turn_point_Num[3] = Croos_turn_point_Num[1] + 1;
					}
				}
				else
				{
					IF_Warning_MOD = 1;
				}
				if (IF_Warning_MOD)
				{
					//无上方转折点，计算斜率，向上拟定边界
					Get_K_To_get_UP_edge(1);
					IF_Warning_MOD = 0;
				}
				else
				{
					TWO_Point_Get_edge_L(L_edge_X[L_edge_Num - 1], L_edge_Y[L_edge_Num - 1], L_edge_X[L_edge_Num], L_edge_Y[L_edge_Num], L_edge_Num - 1);
				}
			}
			else
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(1);
				flag_type_round = 0;
			}
		}
		if (flag_type_round == 2)
		{
			Croos_turn_point_Num[2] = Find_MAX_Y(0, 1, R_edge_Num);
			Croos_turn_point[3] = R_edge_X[Croos_turn_point_Num[2]];
			Croos_turn_point[4] = R_edge_Y[Croos_turn_point_Num[2]];
			//删除错误点
			Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
			//向上补线
			//右边沿向上扫描处理
			Croos_Search_up_R2();
			if (Cross_R_IF_2)
			{
				if (R_edge_Y[0] < IMG_H - Num_lim_search_UP)
				{
					Search_8_R(R_edge_X[R_edge_Num], R_edge_Y[0]);
					if (IF_Warning_MOD)
					{
						Del_Data_from_N_R(Croos_turn_point_Num[2] + 1);
					}
					else
					{
						//删除点（第二段只剩最后一点）
						Del_Data_from_A_to_B_R(Croos_turn_point_Num[2] + 1, R_edge_Num - 1);
						Croos_turn_point_Num[4] = Croos_turn_point_Num[2] + 1;
					}
				}
				else
				{
					IF_Warning_MOD = 1;
				}
				if (IF_Warning_MOD)
				{
					//无上方转折点，计算斜率，向上拟定边界
					Get_K_To_get_UP_edge(0);
					IF_Warning_MOD = 0;
				}
				else
				{
					TWO_Point_Get_edge_R(R_edge_X[R_edge_Num - 1], R_edge_Y[R_edge_Num - 1], R_edge_X[R_edge_Num], R_edge_Y[R_edge_Num], R_edge_Num - 1);
				}
			}
			else
			{
				//无上方转折点，计算斜率，向上拟定边界
				Get_K_To_get_UP_edge(0);
				flag_type_round = 0;
			}
		}
	}

	if (flag_type == 5 && flag_type_round == 0)
	{
		Get_angle_R(1, R_edge_Num);
		R_edge_angle[0] = 0;
		if (R_edge_Num > 6)
		{
			flag_type_Number_judge = 0;
			for (uint8 i = 4; i < R_edge_Num - 2; i++)
			{
				if (R_edge_angle[flag_type_Number_judge] < R_edge_angle[i])
				{
					flag_type_Number_judge = i;
				}
			}
			if (R_edge_angle[flag_type_Number_judge] > angle_is_S)
			{
				R_edge_angle[0] = flag_type_Number_judge;
			}
		}
		Get_angle_L(1, L_edge_Num);
		L_edge_angle[0] = 0;
		if (L_edge_Num > 6)
		{
			flag_type_Number_judge = 0;
			for (uint8 i = 4; i < L_edge_Num - 2; i++)
			{
				if (L_edge_angle[flag_type_Number_judge] < L_edge_angle[i])
				{
					flag_type_Number_judge = i;
				}
			}
			if (L_edge_angle[flag_type_Number_judge] > angle_is_S)
			{
				L_edge_angle[0] = flag_type_Number_judge;
			}
		}
		//执行
		if (R_edge_angle[0])
		{
			Del_Data_from_N_R(R_edge_angle[0] + 1);
			Get_K_To_get_UP_edge(0);
		}
		if (L_edge_angle[0])
		{
			Del_Data_from_N_L(L_edge_angle[0] + 1);
			Get_K_To_get_UP_edge(1);
		}
	}

	//第四步通过两个边沿补全中线
	Mid_Get();
	//绘画中线和边界
	Print_Mid_and_Edge();
	return 1;
}

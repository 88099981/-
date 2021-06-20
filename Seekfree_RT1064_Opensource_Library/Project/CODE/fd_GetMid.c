#include "fd_GetMid.h"

//data
uint8 img[IMG_Y][IMG_X];
EDGE edge[EDGE_MAX];    //边界结构体数组 边界数值为0的时候为丢边
ANGLE angle[EDGE_MAX];  //边沿角度数组
uint8 EdgeNum;    //边沿数
uint8 EdgeLosePos[EDGE_MAX];  //丢边位置 (Y轴           【单帧初始化】
uint8 EdgeLoseNum;   //丢边数                           【单帧初始化】
int16 mid[EDGE_MAX];    //TODO 确定中线数组所需大小      【单帧初始化】
int8 CamError;

//flag
//------------------------工作状态标志,由扫线函数调用------------------------//
uint8 flag_MidStart_in_Black=0;   //Hor_Search 函数中给定起始点在黑区内    【单帧初始化】
uint8 flag_Ver_Search_Finish=0;   //垂直扫描完成                          【单帧初始化】

//-------------------基础标志,原则上仅由扫线函数及状态机调用------------------//
uint8 flag_LoseEdge_part_L=0; //左部分丢边     【单帧初始化】
uint8 flag_LoseEdge_part_R=0; //              【单帧初始化】
uint8 flag_LoseEdge_all_L=0;  //              【单帧初始化】
uint8 flag_LoseEdge_all_R=0;  //              【单帧初始化】
uint8 flag_Straight_L=0;  //左直道    检测直道比检测弯道简单的多            【单帧初始化】
uint8 flag_Straight_R=0;  //右直道                                         【单帧初始化】

//------------------高级标志,原则上由状态机确定补线函数调用-------------------//
uint8 flag_T_Road=0;   //T形路（泛指最顶上几行全黑，以下出现大面积两侧丢边的状况
uint8 flag_Cross=0;    //十字                 【单帧初始化】
uint8 flag_Round_ARM_L=0;  //左环岛预位
uint8 flag_Round_ARM_R=0;  //右环岛预位
uint8 flag_Round_ARM_L_B=0;  //左环岛预位 用于限制flag_Round_ARM_L/R在同一位置多次置位导致对位置的错误判断  【弃用】
uint8 flag_Round_ARM_R_B=0;  //右环岛预位                                                                【弃用】
uint8 flag_Is_This_Round=0;   //环岛决策标志  询问电感(调用完记得复位)    （为什么不问问神奇海螺呢？
uint8 flag_Round_in_L=0;   //左环岛环中 //ATTENTION 从入环开始置位
uint8 flag_Round_in_R=0;   //右环岛环中
uint8 flag_Normal_Lose_L=0;   //一般丢左边                 【单帧初始化】
uint8 flag_Normal_Lose_R=0;   //一般丢右边                 【单帧初始化】
uint8 flag_Garage_L=0;        //车库在左侧
uint8 flag_Garage_R=0;        //车库在右侧
uint8 flag_AprilTag_ARM=0;    //AprilTag预位 置位后进入AprilTag判断函数（判断存在 【单帧初始化】
uint8 flag_AprilTag=0;        //AprilTag存在                                     【单帧初始化】

//--------------------------------故障/辅助标志------------------------------//
uint8 flag_Indct_Help_Me;   //求助电感循迹  调用完记得复位               【单帧初始化】



//******************************* Base Function *********************************/
void init(void)
{
    //****单帧标志位初始化****
    flag_LoseEdge_part_L=0;
    flag_LoseEdge_part_R=0;
    flag_LoseEdge_all_L=0;
    flag_LoseEdge_all_R=0;
    flag_T_Road=0;
    flag_Cross=0;
    flag_MidStart_in_Black=0;
    flag_Ver_Search_Finish=0;
    flag_Indct_Help_Me=0;
    flag_Straight_L=0;
    flag_Straight_R=0;
    flag_Normal_Lose_L=0;
    flag_Normal_Lose_R=0;
    flag_AprilTag_ARM=0;
    flag_AprilTag=0;
    //************************

    for(uint8 i=0;i<EDGE_MAX;i++)
    {
        EdgeLosePos[i]=0;
        edge[i].Lx=0;
        edge[i].Rx=IMG_X-1;
        angle[i].L=180;
        angle[i].R=180;
        mid[i]=0;
    }
}




void Y_Change(void)
{
    uint8 temp;
    for (uint8 i = 0; i < IMG_Y / 2; i++)
    {
        for (uint8 j = 0; j < IMG_X; j++)
        {
            temp = img[i][j];
            img[i][j] = img[IMG_Y - i - 1][j];
            img[IMG_Y - i - 1][j] = temp;
        }
    }
}



//快速开方 （从学长那抄过来的
float Cp_sqrt(float number)
{
	long i;
	float x, y;
	const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = *(long*)&y;	//	取
	i = 0x5f3759df - (i >> 1);  //卡马克
	//i  = 0x5f375a86 - ( i >> 1 );  //Lomont
	y = *(float*)&i;
	y = y * (f - (x * y * y));
	y = y * (f - (x * y * y));
	return number * y;
}



//获取角度 计算向量夹角
float Get_Angle(uint8 p1_y,uint8 p2_y,uint8 Target_y,uint8 l_or_r)
{
    float cos=0;
    float FA_x=0;   //向量FA
    float FA_y=0;
    float FB_x=0;   //向量FB
    float FB_y=0;

    if(l_or_r)
    {
        FA_x=edge[p1_y].Lx-edge[Target_y].Lx;
        FB_x=edge[p2_y].Lx-edge[Target_y].Lx;

        FA_y=p1_y-Target_y;
        FB_y=p2_y-Target_y;
    }
    else
    {
        FA_x=edge[p1_y].Rx-edge[Target_y].Rx;
        FB_x=edge[p2_y].Rx-edge[Target_y].Rx;

        FA_y=p1_y-Target_y;
        FB_y=p2_y-Target_y;
    }

    if(FA_x==0 || FB_x==0 || FA_y==0 || FB_y==0)
    {
        return 0;
    }
    
    cos=(FA_x*FB_x + FA_y*FB_y); //分子
    cos/=Cp_sqrt(FA_x*FA_x + FA_y*FA_y)*Cp_sqrt(FB_x*FB_x + FB_y*FB_y); //分母

    return (acos(cos)*57.295); //转换成角度制输出 别忘记 <math.h>
}




void Del_Edge_Data(uint8 p1_y,uint8 p2_y)
{
    for(uint8 i=p1_y;i<=p2_y;i++)
    {
        edge[i].Lx=0;
        edge[i].Rx=0;
    }
}




//使用前两个点连线的斜率设置目标点
uint8 Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r) //通过因为高度和边沿点是对应的，所以输入纵坐标来表示对应的点
{
    float Slope=0;  //斜率 其实是cot
    
    if(l_or_r)
    {
        Slope=(edge[p1_y].Lx - edge[p2_y].Lx)/(p1_y - p2_y);
        edge[target_p_y].Lx = edge[p1_y].Lx + (uint8)(Slope*target_p_y);
    }
    else
    {
        Slope=(edge[p1_y].Rx - edge[p2_y].Rx)/(p1_y - p2_y);
        edge[target_p_y].Rx = edge[p1_y].Lx + (uint8)(Slope*target_p_y);
    }

    return 1;
}



//连接两点 通过因为高度和边沿点是对应的，所以输入纵坐标来表示对应的点 //ATTENTION p1_y小于p2_y
uint8 Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y)
{
    float Slope=0;   //斜率 其实是cot
    if(l_or_r)
    {
        if((p1_y-p2_y) != 0)  //垂直的时候就不用算了
        {
            Slope=(Target[p2_y].Lx-Target[p1_y].Lx)/(p2_y-p1_y);

            for(int i=1;i<=p2_y-p1_y;i++)
            {
                Target[p1_y+i].Lx=(uint8)(i*Slope+Target[p1_y].Lx);
            }
        }

        return 1;
    }
    else
    {
        if((p1_y-p2_y) != 0)  //垂直的时候就不用算了
        {
            Slope=(Target[p2_y].Rx-Target[p1_y].Rx)/(p2_y-p1_y);

            for(int i=1;i<=p2_y-p1_y;i++)
            {
                Target[p1_y+i].Rx=(uint8)(i*Slope+Target[p1_y].Rx);
            }
        }

        return 1;
    }

    return 0;
}




//连接两点 通过因为高度和边沿点是对应的，所以输入纵坐标来表示对应的点    //ATTENTION p1_y小于p2_y
uint8 Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y) 
{
    float Slope=0;   //斜率 其实是cot
    if((Target[p1_y]-Target[p2_y]) != 0)  //垂直的时候就不用算了
    {
        Slope=(p1_y-p2_y)/(Target[p1_y]-Target[p2_y]);

        for(int i=1;i<=p2_y-p1_y;i++)
        {
            Target[p1_y+i]=(uint8)(i*Slope+Target[p1_y]);
        }
    }
    return 1;
}


//水平扫线（最底层） //ATTENTION y为相对限制底框的高度 y为内部递归(可能)使用的参数，外部调用切勿赋值
uint8 Hor_Search_Base(uint8 MidStart,uint8 y)
{
    flag_MidStart_in_Black=0;   //标志位置零

    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //范围验证
    {

        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //如果给定的中点在黑区内
    {


        uint8 Sum_of_white_zone=0;  //当前行白区个数 默认最多两个 修改个数请注意同时修改temp_edge数组的元素个数,以及其它的一堆都要改
        EDGE temp_edge;  //临时边界点

        temp_edge.Lx=0;  
        temp_edge.Rx=IMG_X-1;

        for(uint8 i=MidStart;i>=X_LIMIT;i--)    //向左搜索
        {
            if(Sum_of_white_zone!=1 &&img[y+Y_LIMIT][i]==White)    //扫到白区就不再修改右边沿
            {
                temp_edge.Rx=i;
                Sum_of_white_zone=1;    //标志已经向左找到白区
            }

            if(Sum_of_white_zone==1 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge.Lx=i;
            }
        }

        for(uint8 i=MidStart;i<IMG_X-X_LIMIT;i++)    //向右搜索
        {
            if(Sum_of_white_zone!=2 && img[y+Y_LIMIT][i]==White)    //扫到白区就不再修改左边沿
            {
                temp_edge.Lx=i;
                Sum_of_white_zone=2;    //标志已经向右找到白区
            }

            if(Sum_of_white_zone==2 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge.Rx=i;
            }
        }

        if(!Sum_of_white_zone)  //没白区
        {
            flag_Indct_Help_Me=1;   //请求切换到电感循迹

            return 0;
        }


        MidStart=(temp_edge.Rx+temp_edge.Lx)/2;

    }

    for(uint8 i=MidStart;i>=X_LIMIT;i--)   //向左搜索
    {
        if(img[y+Y_LIMIT][i]==Black)  //TODO 改进判断噪点的方法
        {
            edge[y].Lx=i;                         //注意！丢线直接为默认值0 后面程序也是根据0判断边沿是否为特殊情况的
            break;
        }

        if(i==X_LIMIT)  //到边缘仍未找到黑区 判断丢线
        {
            flag_LoseEdge_part_L++;
        }
    }

    for(uint8 i=MidStart;i<=IMG_X-X_LIMIT;i++)   //向右搜索
    {
        if(img[y+Y_LIMIT][i]==Black)
        {
            edge[y].Rx=i;
            break;
        }

        if(i==IMG_X-X_LIMIT)
        {
            flag_LoseEdge_part_R++;
        }
    }

    if(edge[y].Lx==0)   //底层丢边 那就先拿图像边沿凑数吧
    {
        edge[y].Lx=X_LIMIT;
    }

    if(edge[y].Rx==0)
    {
        edge[y].Rx=IMG_X-X_LIMIT;
    }

    MidStart=(edge[0].Lx+edge[0].Rx)/2;

    return 0;
}



//水平扫线（单行） //ATTENTION y为相对限制底框的高度
uint8 Hor_Search(uint8 MidStart,uint8 y)
{
    flag_MidStart_in_Black=0;   //标志位置零

    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //范围验证
    {
        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //如果给定的中点在黑区内
    {
        flag_MidStart_in_Black=1;
        return 0;
    }

    for(uint8 i=MidStart;i>=X_LIMIT;i--)   //向左搜索
    {
        if(img[y+Y_LIMIT][i]==Black) 
        {
            edge[y].Lx=i;                         //TODO 确定该注释的正确性-> 注意！丢线直接为默认值0 后面程序也是根据0判断边沿是否为特殊情况的
            break;
        }

        if(i==X_LIMIT)  //到边缘仍未找到黑区 判断丢线
        {
            flag_LoseEdge_part_L++;
        }
    }

    for(uint8 i=MidStart;i<=IMG_X-X_LIMIT;i++)   //向右搜索
    {
        if(img[y+Y_LIMIT][i]==Black)
        {
            edge[y].Rx=i;
            break;
        }

        if(i==IMG_X-X_LIMIT)
        {
            flag_LoseEdge_part_R++;
        }
    }

    if(edge[y].Rx-edge[y].Lx<=4)    //可能是斑马线
    {
        EdgeNum=y;  //限制最大高度，避免对中线及偏差产生影响
    }

    if(y>3) // 待边沿稳定后
    {
        if(edge[y].Lx-edge[y-2].Lx>=10 || edge[y].Lx-edge[y-2].Lx<=-10) //ATTENTION EdgeLosePos可能会储存相同的高度（双侧丢边时）
        {
            edge[y].Lx=0;   //突变视作丢边
            EdgeLosePos[EdgeLoseNum++]=y;    //储存丢边信息
        }

        if(edge[y].Rx-edge[y-2].Rx>=10 || edge[y].Rx-edge[y-2].Rx<=-10)
        {
            edge[y].Rx=0;   //突变视作丢边
            EdgeLosePos[EdgeLoseNum++]=y;    //储存丢边信息
        }
    }

    if(y)   //不处理底边
    {
        if(edge[y].Lx==0 && edge[y].Rx!=0)
        {
            flag_LoseEdge_part_L++;
            //edge[y].Lx=edge[y].Rx-Width_Cali(y);
        }

        else if(edge[y].Rx==0 && edge[y].Lx!=0)  //
        {
            flag_LoseEdge_part_R++;
            //edge[y].Rx=edge[y].Lx+Width_Cali(y);
        }
        else if(edge[y].Rx==0 && edge[y].Lx==0) //坑爹的两侧丢边    
        {
            flag_LoseEdge_part_L++;
            flag_LoseEdge_part_R++;
        }
    }


    return ((edge[y].Rx-edge[y].Lx)/2);
}



//******************************* Base Function *********************************/


//*******************************************************************************/

 //初始垂直扫线
uint8 Ver_Search(uint8 MidStart)
{
    EdgeNum=0;
    flag_Ver_Search_Finish=0;

    for(uint8 i=Y_LIMIT;i<=IMG_Y-Y_LIMIT;i++)
    {
        EdgeNum=i;  //设定垂线高度 （扫线最大高度） EdgeNum 修正为数组长度 从Y_LIMIT开始
        if(img[i][MidStart]==Black)
        {
            flag_Ver_Search_Finish=1;
            break;
        }
    }

    //EdgeNum-=Y_LIMIT;

    return(EdgeNum);
}




//垂直扫线 通用垂直扫线 不会改变EdgeNum的值   *注意！ 这个函数是从最底边开始扫描的 //（别问为啥不写进一个函数里，就是懒
uint8 Uni_Ver_Search(uint8 MidStart)
{
    uint8 IN_EdgeNum=0;

    for(uint8 i=0;i<=IMG_Y-Y_LIMIT;i++)
    {
        IN_EdgeNum=i;  //设定垂线高度 （扫线最大高度） EdgeNum 修正为数组长度 从Y_LIMIT开始
        if(img[i][MidStart]==Black)
        {
            flag_Ver_Search_Finish=1;
            break;
        }
    }

    IN_EdgeNum-=Y_LIMIT*2;  //从底边开始扫的 减去双倍的边缘高度

    return(IN_EdgeNum);
}





void If_Straight(void)
{
    uint8 SearchTimesMax = EdgeNum - CUR_RESOLUTION; //最大搜索次数

    flag_Straight_L = 1; //先假设为直道
    flag_Straight_R = 1;

    for (uint8 i = 0; i < SearchTimesMax; i++) //ATTENTION 此种方法无法计算到最底下和最顶端CUR_RESOLUTION/2个边沿的角度
    {
        angle[i].L = Get_Angle(i, i + CUR_RESOLUTION, i + CUR_RESOLUTION / 2, 1);
        angle[i].R = Get_Angle(i, i + CUR_RESOLUTION, i + CUR_RESOLUTION / 2, 0);
    }

    for (uint8 i = 0; i < SearchTimesMax; i++)
    {
        if (angle[i].L <= 160) //以160度为界 180是标准直线
        {
            flag_Straight_L = 0;
        }

        if (angle[i].R <= 160)
        {
            flag_Straight_R = 0;
        }
    }
}





//特征比较函数，将特征数组和图像对应位置进行比较，返回相似度(0~100) //ATTENTION T_x T_y为左下角坐标,另外注意特征数组不要小于dx*dy
uint8 Feature_Verify(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 *feature)    
{
    float rate=0;

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //范围检查
    {
        return(101);
    }

    for(uint8 i=0;i<dy;i++)
    {
        for(uint8 j=0;j<dx;j++)
        {
            if(img[T_y+i][T_x+j]==feature[i*dx+j])    //与特征数组/图像比较
            {
                rate++;
                img[T_y+i][T_x+j]=Gray;

                #ifdef IMG_DEBUG    //TODO 要求在菜单程序中加入图像调试对应的标志位以代替宏定义
                
                #endif
            }
            //img[T_y+i][T_x+j]=Gray;
        }
    }

    rate=(rate/(dx*dy))*100;

    return((uint8)rate);
}




//特征比较函数(方框），thickness为方框厚度，将特征数组和图像对应位置进行比较，返回相似度(0~100) 
uint8 Feature_Verify_Box(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 thickness,uint8 white_or_black)
{
    float rate=0;
    uint8 feature=white_or_black?White:Black; //确定需比较的颜色

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //范围检查
    {
        return(101);
    }

    for(uint8 i=0;i<dy;i++)
    {
        for(uint8 j=0;j<dx;j++)
        {
            if(i<thickness || i>=dy-thickness)
            {
                if(img[T_y+i][T_x+j]==feature)
                rate++;

                #ifdef IMG_DEBUG
                img[T_y+i][T_x+j]=Gray;
                #endif
            }
            else
            {
                if(j<thickness || j>=dx-thickness)
                {
                    if(img[T_y+i][T_x+j]==feature)
                    rate++;

                    #ifdef IMG_DEBUG
                    img[T_y+i][T_x+j]=Gray;
                    #endif
                }
            }
        }
    }

    rate=(rate/(dx*dy-(dx-2*thickness)*(dy-2*thickness)))*100;
		
		return((uint8)rate);
}



//判断斑马线（车库）
uint8 If_Garage(void)
{
    uint8 StripeNum=0;  //条纹数

    for(uint8 i=0;i<IMG_X-1;i++)
    {
        if(img[3][i]==White && img[3][i+1]==Black)
        {
            StripeNum++;
        }
    }

    if(StripeNum>=6)    //条纹数大于设定值
    {
        #ifdef IMG_DEBUG
        for(uint8 i=0;i<IMG_X;i++)  //绘画该线以便调试
        {
            img[3][i]=Gray;
        }
        #endif

        if(Feature_Verify(0,0,20,10,Block_A)>=90)
        {
            flag_Garage_L=1;
            flag_Garage_R=0;    //因为没有每帧初始化，所以为了避免误置造成矛盾，故对两个标志位均置高
            return 1;
        }
        else if(Feature_Verify(167,0,20,10,Block_A)>=90)
        {
            flag_Garage_L=0;
            flag_Garage_R=1;
            return 1;
        }

        for(uint8 i=0;i<IMG_X;i++)
        {
            img[3][i]=Gray;
        }
    }

    return 0;
}



//状态机
uint8 Judge(void)
{
    //-------状态整理 <head>--------//
    
    if(flag_Round_ARM_L)
    {
        flag_Round_ARM_L--;
    }
    else if(flag_Round_ARM_R)
    {
        flag_Round_ARM_R--;
    }
   
    //-------状态整理 <bottom>--------//

    //------车库检测 <head>---------//
    if(If_Garage())
    {
        bb_time=20;
        return 1;
    }
    //------车库检测 <bottom>---------//

     //-------双侧丢边 <head>--------//
    if(flag_LoseEdge_part_L*flag_LoseEdge_part_R != 0)  //双侧丢边判断为十字
    {
        uint16 NumInBlack=0;

        for(uint8 i=IMG_Y-1;i>IMG_Y-3;i--)  //扫两行
        {
            for(uint8 j=0;j<IMG_X;j++)
            {
                if(img[i][j]==Black)
                {
                    NumInBlack++;
                }

                #ifdef IMG_DEBUG
                img[i][j]=Gray;
                #endif
            }
        }

        if(NumInBlack>=IMG_X*1.9) //额最顶上两行基本全黑 这个值根据需求修改
        {
            flag_T_Road=1;
            return 1;
        }

        flag_Cross=1;   //T形路判断优先级高于十字

        return 1;
    }
    //-------双侧丢边 <bottom>--------//

    //------环岛检测 <head>---------//
    do
    {
        if(Feature_Verify(83,20,20,20,Block_A)<=90) //判断是否为直赛道，减小直角弯的误报
        {
            break;
        }

        if(Feature_Verify(0,19,50,10,Block_A)>=90)  //因为有的大环岛入环比较柔和，没有丢边，所以在丢边外判断
        {
            //ATTENTION 无法独立进行入环判断(需要电感辅助)
            flag_Round_ARM_L=ROUND_ARM_COUNT_TIMES; //当该标志位不为零时，都应该用电感验证

            //return 1;
        }
        else if(Feature_Verify(137,19,50,10,Block_A)>=90)
        {
            flag_Round_ARM_R=ROUND_ARM_COUNT_TIMES; //当该标志位不为零时，都应该用电感验证

            //return 1;
        }
    } while (0);    //想写goto又不敢写的屑
    
    if(zuo_yuanhuan_flag && flag_Round_ARM_L)    //入环阶段拉线    ruhuan>=hd_in 为外部参数
    {
        uint8 addition=80;

        for(uint8 i=EdgeNum;i>0;i--)
        {
            addition*=0.96;
            edge[i].Rx-=addition;
        }
        /*
        uint8 Target_y=0;
        uint8 Target_x[2]={0};  //第一元素为当前值，第二元素为上次值

        for(uint8 i=EdgeNum;i>=IMG_Y/4;i--)   //入左环，从上到下，从右到左扫线，寻找尖点
        {   
            Target_x[1]=Target_x[0];

            for(uint8 j=edge[i].Rx;j>IMG_X/3;j--)
            {
                if(img[i][j-1]==Black && img[i][j]==White)
                {
                    Target_x[0]=j;
                    break;
                }
            }

            Target_y=i;

            if(i<EdgeNum-1 && Target_x[1]-Target_x[0]>=15) //变化较大
            {
                break;
            }
        }

        if(Target_y)
        {
            //edge[0].Rx=IMG_X/2;
            edge[Target_y].Rx=Target_x[0];
            Connect(edge,0,0,Target_y);

            flag_Normal_Lose_L=1;
        }
        */
    }
    else if(you_yuanhuan_flag && flag_Round_ARM_R)   //ruhuan>=hd_in 为外部参数，此时已经过了第一个环口
    {
        uint8 addition=80;

        for(uint8 i=EdgeNum;i>0;i--)
        {
            addition*=0.96;
            edge[i].Lx+=addition;
        }
        /*
        uint8 Target_y=0;
        uint8 Target_x[2]={0};  //第一元素为当前值，第二元素为上次值

        for(uint8 i=EdgeNum;i>=IMG_Y/4;i--)   //入右环，从上到下，从左到右扫线，寻找尖点
        {   
            Target_x[1]=Target_x[0];

            for(uint8 j=edge[i].Lx;j<IMG_X-1;j++)
            {
                if(img[i][j]==Black && img[i][j+1]==White)
                {
                    Target_x[0]=j;
                    break;
                }
            }

            Target_y=i;

            if(i<EdgeNum-1 && Target_x[0]-Target_x[1]>=15) //变化较大
            {
                break;
            }
        }

        if(Target_y)
        {
            //edge[0].Lx=IMG_X/2;
            edge[Target_y].Lx=Target_x[0];
            Connect(edge,1,0,Target_y);

            flag_Normal_Lose_R=1;
        }
        */
    }

    //------环岛检测 <bottom>---------//

    //------AprilTag检测 <head>---------//
    if(Feature_Verify_Box(64,3,60,17,2,1)>=75 && Feature_Verify(66,5,56,13,Block_B)>=20)
    {
        bb_time=20;
        return 1;
    }
    //------AprilTag检测 <bottom>---------//

    //-------单侧丢边 <head>--------//
    if(flag_LoseEdge_part_L!=0 && flag_LoseEdge_part_R==0) //单左丢边
    {
        flag_Normal_Lose_L=1;   //普通单左丢边
        return 1;
    }
    else if(flag_LoseEdge_part_L==0 && flag_LoseEdge_part_R!=0) //单右丢边
    {
        flag_Normal_Lose_R=1;   //普通单右丢边
        return 1;
    }

    //-------单侧丢边 <bottom>--------//


    return 0;
}



//根据高度计算赛道宽度
inline uint8 Width_Cali(uint8 y)
{
    if(0)   //暂时改为标定值计算
        return((edge[0].Rx-edge[0].Lx)-(uint8)(WIDTH_K*y)); //使用最底层宽度计算
    else
        return(WIDTH_BASE-(uint8)(WIDTH_K*y)); //使用标定值计算
}



//双侧丢边
void Damn_Lose_Edge_all(void)
{

uint8 flag_Edge_Start_Exist=0;
uint8 flag_Edge_End_Exist=0;
uint8 EdgeStartPos=0;
uint8 EdgeEndPos=0;

for(uint8 i=0;i<EdgeNum;i++)
{
    if(edge[i].Lx!=0 && edge[i].Rx!=0)  //判断是否存在边沿
    {
        flag_Edge_Start_Exist=1;
        EdgeStartPos=i;
        break;
    }
}

for(uint8 i=EdgeNum;i>0;i--)
{
    if(edge[i].Lx!=0 && edge[i].Rx!=0)  //判断是否存在边沿
    {
        flag_Edge_End_Exist=0;
        EdgeEndPos=i;
        break;
    }
}

if(flag_Edge_Start_Exist && flag_Edge_End_Exist)
{
    Connect(edge,1,EdgeStartPos,EdgeEndPos);
    Connect(edge,2,EdgeStartPos,EdgeEndPos);
}
    /*
    uint8 flag_Ver_Raise=0; //垂直高度激增
    uint8 flag_Ver_Drop=0;  //垂直高度跌落

    uint8 Ver_Raise_X=0;
    uint8 Ver_Drop_X=0;
    uint8 ver[camera_raw_data_X-X_LIMIT+1]={0}; //垂直高度数组
    uint8 Lose_Edge_Sum=0;  //丢边数
    uint8 Max_Ver=0;
    uint8 Max_Ver_X=0;  //最长垂线的横坐标
    uint8 temp=0;

    for(uint8 i=X_LIMIT;i<=camera_raw_data_X-X_LIMIT;i++)
    {   
        ver[i-X_LIMIT]=Uni_Ver_Search(i);   //存入数据
    }

    for(uint8 i=X_LIMIT;i<=camera_raw_data_X-X_LIMIT;i++)
    {
        if((i-X_LIMIT-5) >= 0)
        {
            if((ver[i-X_LIMIT]-ver[i-X_LIMIT-5]) >= 10)
            {
                Ver_Raise_X=i-X_LIMIT-5;
            }

            if((ver[i-X_LIMIT-5]-ver[i-X_LIMIT]) >= 10)
            {
                Ver_Drop_X=i-X_LIMIT;
            }
        }
    }

    for(uint8 i=0;i<=EdgeNum;i++)
    {
        if(Ver_Drop_X!=0 && Ver_Raise_X!=0) //两边都有高度突变
        {
            mid[i]=(Ver_Drop_X+Ver_Raise_X)/2;   //暂时用该值补中线
        }
        else if(Ver_Drop_X==0 && Ver_Raise_X!=0)
        {
            mid[i]=(Ver_Raise_X+0)/2;
        }
        else if(Ver_Drop_X!=0 || Ver_Raise_X==0)
        {
            mid[i]=(Ver_Drop_X+(camera_raw_data_X-X_LIMIT))/2;
        }
    }

    for(uint8 i=0;i<=EdgeNum;i++)
    {
        edge[i].Lx=mid[i]-Width_Cali(i)/2;  //FIXME 未考虑负值情况
        edge[i].Rx=mid[i]+Width_Cali(i)/2;
    }

    qDebug()<<"Drop:"<<Ver_Drop_X<<" Raise:"<<Ver_Raise_X<<"\n";
    

   for(int i=0;i<=EdgeNum;i++)
   {
       for(int j=0;j<EdgeLoseNum;j++)  //觉得比较麻烦 就直接嵌套了
       {
           if(i==EdgeLosePos[j])    //丢边
           {
               if(edge[i-1].Lx!=0)  //避免最底下就是白区 写的稀烂
               {
                   edge[i].Lx=edge[i-1].Lx;
               }
               
               if(edge[i-1].Rx!=0)
               {
                   edge[i].Rx=edge[i-1].Rx;
               }

           }
       }
   }
    */

}



//丢边补全
void If_Lose_Edge(void)
{
    if(flag_Garage_L)
    {
        /*
        edge[0].Rx=IMG_X-1; //设置右边沿的数据
        edge[IMG_Y-1].Rx=0;

        Connect(edge,0,0,IMG_X-1);  //车库在左侧，将右侧边缘设为左上到右下的斜线
        EdgeNum=IMG_Y/2;    //中点数为最大高度一半
        flag_Normal_Lose_L=1;   //让其补右侧线
        */
    }
    else if(flag_Garage_R)
    {
        /*
        edge[0].Lx=0; //设置右边沿的数据
        edge[IMG_Y-1].Lx=IMG_X-1;

        Connect(edge,1,0,IMG_X-1);
        EdgeNum=IMG_Y/2;    //中点数为最大高度一半
        flag_Normal_Lose_R=1;   //让其补右侧线
        */
    }

    if(flag_T_Road)
    {
        if(zuo_yuanhuan_flag)
        {
            edge[0].Rx=120;
            edge[EdgeNum].Rx=20;
            Connect(edge,0,0,EdgeNum);
            flag_Normal_Lose_L;
        }
        else if(you_yuanhuan_flag)
        {
            edge[0].Lx=68;
            edge[EdgeNum].Lx=168;
            Connect(edge,1,0,EdgeNum);
            flag_Normal_Lose_R;
        }
    }

    if(flag_Cross)  //十字
    {
        Damn_Lose_Edge_all();   //双侧丢边函数
    }
    else if(flag_Normal_Lose_L)  //单左丢边
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Lx=edge[i].Rx-Width_Cali(i);    //删边补全  （直接覆写左边界数据
        }
    }
    else if(flag_Normal_Lose_R)  //单右丢边
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Rx=edge[i].Lx+Width_Cali(i);
        }
    }
}




//计算中点&更新为折线(减少意外出错)
void Set_Mid(void)
{
    for(uint8 i=0;i<=EdgeNum;i++)   //计算中点
    {
        if((edge[i].Lx * edge[i].Rx) != 0)  //对于未丢边的情况
        {
            mid[i]=(edge[i].Lx+edge[i].Rx)/2;
        }

        else if(edge[i].Lx==0 && edge[i].Rx!=0)  //
        {
            mid[i]=(edge[i].Lx+edge[i].Rx)/2;
        }

        else if(edge[i].Rx==0 && edge[i].Lx!=0)  //
        {
            mid[i]=(edge[i].Lx+edge[i].Rx)/2;
        }
        
        if(i>=5)    //待中线稳定后，对超过容差的相邻中线进行判断
        {
            if(mid[i]-mid[i-1]>=20 || mid[i]-mid[i-1]<=-20)
            {
                EdgeNum=i-1;  //结束中点计算
            }
        }
        
    }

    
    for(uint8 i=1;i<(EdgeNum/MID_RESOLUTION)-1;i++)   //中线拟合成直线
    {
        Mid_Connect(mid,(i+1)*MID_RESOLUTION,i*MID_RESOLUTION);
    }
    

}



//简单检错程序
void Simple_Err_Check(void) 
{

}




//绘画中线
void Print_Mid(void)
{
    for(uint8 i=Y_LIMIT; i<=EdgeNum+Y_LIMIT; i++)
    {
        img[i][edge[i-Y_LIMIT].Lx]=Gray;
        img[i][edge[i-Y_LIMIT].Rx]=Gray;

        if(mid[i]<IMG_X && mid[i]>0)    //我也不知道为什么当时要写这个奇怪的限制
            img[i][mid[i-Y_LIMIT]]=Gray;
    }

}



//坑爹的扫线主函数
void Search(void)
{

    uint8 MidStart=IMG_X/2;   //底边搜索起始点横坐标    
    //讲道理，我觉得我不应该把search里和hor_search里搞一个同名的局部变量，但是懒得改了

    init();
    Y_Change();

    //-------------------扫线部分 <head>--------------//
    Hor_Search_Base(MidStart,0);   //第一遍先确定扫描基准中线
    MidStart=(edge[0].Lx+edge[0].Rx)/2;

    Ver_Search(MidStart);   //垂直扫线 更新扫线最大高度 EdgeNum

    for(uint8 i=0;i<=EdgeNum;i++)   //水平扫线循环
    {
        Hor_Search(MidStart,i);
                                //TODO 缺一个对丢边格数的判断来对 flag_LoseEdge_all_X置位
    }

    //If_Straight();  //确定直边

    
    //-------------------扫线部分 <bottom>-------------//


    //-------------------判断部分 <head>---------------//
    Judge();
    //-------------------判断部分 <bottom>-------------//


    //-------------------补线部分 <head>---------------//
    If_Lose_Edge(); 

    Set_Mid();
    //-------------------补线部分 <bottom>-------------//


    //-------------------绘画部分 <head>---------------//
    Print_Mid();
    //-------------------绘画部分 <bottom>-------------//

    //-------------------结束部分 <head>---------------//
    if(flag_Garage_L || flag_Garage_R)
    {
        break_flag=1;
    }
    //-------------------结束部分 <bottom>-------------//
}




void CAM_Error(void)
{
    uint8 unit=EdgeNum/5;

    if(flag_Indct_Help_Me)  //出错的话偏差值设为0就好了
    {
        CamError=0;
    }
    else
    {
        CamError=(IMG_X/2)-(mid[unit]+mid[unit*2]+mid[unit*3]+mid[unit*4]+mid[unit*5-3])/5; //最顶上的边沿可信度较低
    }
    
}

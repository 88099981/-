#include "fd_GetMid.h"

//data
uint8 temp1; //调试时使用的全局变量
uint8 temp2; //调试时使用的全局变量
uint8 img[IMG_Y][IMG_X];
uint8 pix_img[PIX_IMG_Y][PIX_IMG_X];
uint8 copy_pix_img[PIX_IMG_Y][PIX_IMG_X];
EDGE edge[EDGE_MAX];    //边界结构体数组 边界数值为0的时候为丢边
ANGLE angle[EDGE_MAX];  //边沿角度数组
DIRECTION Direction_4[]={{-1,0},{1,0},{0,1},{0,-1}};    //4连通域偏移值
DIRECTION Direction_8[]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};    //8连通域偏移值
uint8 EdgeNum;    //边沿数
uint8 EdgeLosePos[EDGE_MAX];  //丢边位置 (Y轴           【单帧初始化】
uint8 EdgeLoseNum;   //丢边数                           【单帧初始化】
int16 mid[EDGE_MAX];    //TODO 确定中线数组所需大小      【单帧初始化】
uint8 MidStart=IMG_X/2;   //底边搜索起始点横坐标
uint16 Round_ad_limit=600;  //入环ad阈值
uint16 Croess_ad_limit=120;
uint32 RunningCount=0;
uint16 RoundInCount=0; //入环计数
uint8 RoundOutCount=0;
uint8 YRoadInCount=0;
uint8 CrossInCount=0;
uint16 SumInCD=0;   //连通域内点个数(像素化后)
SUMINCD sumincd={0,0};


//flag

//-------------------基础标志,原则上仅由扫线函数及状态机调用------------------//
uint8 flag_LoseEdge_part_L=0; //左部分丢边     【单帧初始化】
uint8 flag_LoseEdge_part_R=0; //              【单帧初始化】
uint8 flag_LoseEdge_all_L=0;  //              【单帧初始化】
uint8 flag_LoseEdge_all_R=0;  //              【单帧初始化】
uint8 flag_Straight_L=0;  //左直道    检测直道比检测弯道简单的多            【单帧初始化】
uint8 flag_Straight_R=0;  //右直道                                         【单帧初始化】

//------------------高级标志,原则上由状态机确定补线函数调用-------------------//
uint8 flag_Straight=0;
uint8 flag_T_Road=0;   //T形路（泛指最顶上几行全黑，以下出现大面积两侧丢边的状况
uint8 flag_Y_Road=0;   //三岔
uint16 flag_Y_Road_IN=0; //三岔中
uint8 Y_Road_Status=1;  //三岔左    //ATTENTION 由openart设置 0右1左
uint8 flag_Cross=0;    //十字                 【单帧初始化】
uint8 Round_Status=0; //0啥事没有 1第一次看到环口 3看到黑区且电感大于阈值 5第二次看到环口 7环内 9出环 (同理右为偶)
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
    temp1=0;
    temp2=0;
    flag_LoseEdge_part_L=0;
    flag_LoseEdge_part_R=0;
    flag_LoseEdge_all_L=0;
    flag_LoseEdge_all_R=0;
    flag_Y_Road=0;
    flag_T_Road=0;
    flag_Cross=0;
    flag_Indct_Help_Me=0;
    flag_Straight_L=0;
    flag_Straight_R=0;

    flag_Normal_Lose_L=0;
    flag_Normal_Lose_R=0;
    flag_AprilTag_ARM=0;
    flag_AprilTag=0;
    flag_Straight=0;
    sumincd.AprilTag=0;
    sumincd.lane=0;
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

}





uint8 Connect_pp(uint8 l_or_r,uint8 p1_x,uint8 p1_y,uint8 p2_x,uint8 p2_y)
{
    float Slope=0;
    if(p2_y==p1_y)
    {
        Slope=0;
    }
    else
    {
        Slope=(p2_x-p1_x)/(p2_y-p1_y);
    }

    if(l_or_r)
    {

        for(uint8 i=p1_y;i<=p2_y;i++)
        {
            edge[i].Lx=(int16)(p1_x+i*Slope);
        }

        return 1;
    }
    else
    {

        for(uint8 i=p1_y;i<=p2_y;i++)
        {
            edge[i].Rx=(int16)(p1_x+i*Slope);
        }

        return 1;
    }
}



//连接两点 通过因为高度和边沿点是对应的，所以输入纵坐标来表示对应的点    //ATTENTION p1_y小于p2_y
uint8 Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y) 
{
    float Slope=0;   //斜率 其实是cot

    if(p2_y==p1_y)
    {
        Slope=0;
    }
    else
    {
        Slope=(Target[p2_y]-Target[p1_y])/(p2_y-p1_y);
    }

    for(uint8 i=p1_y;i<=p2_y;i++)
    {
        Target[i]=Target[p1_y]+(int16)(i*Slope);
    }

    /*
    if((Target[p1_y]-Target[p2_y]) != 0)  //垂直的时候就不用算了
    {
        Slope=(p1_y-p2_y)/(Target[p1_y]-Target[p2_y]);

        for(int i=1;i<=p2_y-p1_y;i++)
        {
            Target[p1_y+i]=(uint8)(i*Slope+Target[p1_y]);
        }
    }
    */

    return 1;
}


//水平扫线（最底层） //ATTENTION y为相对限制底框的高度 y为内部递归(可能)使用的参数，外部调用切勿赋值
uint8 Hor_Search_Base(uint8 MidStart,uint8 y)
{

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
    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //范围验证
    {
        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //如果给定的中点在黑区内
    {
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
    /*
    if(edge[y].Rx-edge[y].Lx<=4)    //可能是斑马线
    {
        EdgeNum=y;  //限制最大高度，避免对中线及偏差产生影响
    }
    */
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
        if(edge[y].Lx==0)
        {
            flag_LoseEdge_part_L++;
        }
        else if(edge[y].Rx==0)
        {
            flag_LoseEdge_part_R++;
        }
    }

    return 0;
}



uint8 zoomin(float weight,uint8 PIX_DeBUG)
{
    uint16 pixsum=0;
    uint16 pixsum2=0;

    for(uint8 i=1,m=0;i<IMG_X-1;i+=3,m++)
    {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
        for(uint8 j=1,n=0;j<IMG_Y-1;j+=3,n++)
        {
            pixsum=img[j][i]*weight;

            for(uint8 k=0;k<8;k++)
            {
                pixsum2+=(uint8)(img[j+Direction_8[k].offset_y][i+Direction_8[k].offset_x]*(1-weight));
            }

            pixsum2/=8;
            pixsum+=pixsum2;

            if(pixsum>0xdf)
            {
                pixsum=0xff;
            }
            else
            {
                 pixsum=0x00;
            }

            //  在处理之前启用该段会导致原图数据被破坏
            if(PIX_DeBUG)
            {
                img[j][i]=pixsum;

                for(uint8 k=0;k<8;k++)
                {
                    img[j+Direction_8[k].offset_y][i+Direction_8[k].offset_x]=(uint8)pixsum;
                }
            }

            pix_img[n][m]=pixsum;
        }
    }

    return 1;
}
//******************************* Base Function *********************************/


//*******************************************************************************/

 //初始垂直扫线
uint8 Ver_Search(uint8 MidStart)
{
    EdgeNum=0;

    for(uint8 i=Y_LIMIT;i<=IMG_Y-Y_LIMIT;i++)
    {
        EdgeNum=i;  //设定垂线高度 （扫线最大高度） EdgeNum 修正为数组长度 从Y_LIMIT开始
        if(img[i][MidStart]==Black)
        {
            break;
        }
    }

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
                //img[T_y+i][T_x+j]=Gray;
            }
        }
    }

    rate=(rate/(dx*dy))*100;

    return((uint8)rate);
}




//特征比较函数(颜色),色彩范围0-255,expect_rate期望率0~100
uint8 Feature_Verify_Color(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 color,float expect_rate)    
{
    uint16 rate=0;
    uint16 unexpect_rate=(uint16)(((100-expect_rate)/100)*(dx*dy));

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //范围检查
    {
        return(101);
    }

    for(uint8 i=0;i<dy;i++)
    {
        for(uint8 j=0;j<dx;j++)
        {
            if(img[T_y+i][T_x+j]!=color)    //与特征数组/图像比较
            {
                rate++;

                if(rate-unexpect_rate>0)  //快速模板匹配，不符特征个数超限时即退出
                {
                    return 0;
                }
                //img[T_y+i][T_x+j]=Gray;
            }
        }
    }

    /*
    rate=(dx*dy)-rate;
    rate=(rate/(dx*dy))*100;

    if(rate)
    return((uint8)rate);
    */
   return 1;    //运行到此处时说明已符合要求
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

                //img[T_y+i][T_x+j]=Gray;

            }
            else
            {
                if(j<thickness || j>=dx-thickness)
                {
                    if(img[T_y+i][T_x+j]==feature)
                    rate++;

                    //img[T_y+i][T_x+j]=Gray;
                }
            }
        }
    }

    rate=(rate/(dx*dy-(dx-2*thickness)*(dy-2*thickness)))*100;
		
		return((uint8)rate);
}



//复制像素数组
void Copy_pix_img(void)
{
	for(uint8 i=0;i<PIX_IMG_Y;i++)  //快速拷贝一份新的像素化图像，以免查找连通域时产生污染
	{
            memmove(&copy_pix_img[i][0],&pix_img[i][0],PIX_IMG_X);
	}
}





//设置赛道连通域生成点 返回u16类型，高八位为横坐标x，低八位为纵坐标y
uint8 SetSeed_Lane(uint8 MidStart,uint8 EdgeNum)
{
    uint8 Seed_x=MidStart/3;
    uint8 Seed_y=EdgeNum/4;

    if(copy_pix_img[Seed_y][Seed_x]==White)
    {
        uint8 SumWhite=0;
        for(uint8 i=0;i<8;i++)
        {
            if(copy_pix_img[Seed_y+Direction_8[i].offset_y][Seed_x+Direction_8[i].offset_x]==White)
            {
                SumWhite++;
            }
        }
        
        if(SumWhite>=5)
        {
            return 1;
        }
    }
    
    return 0;
}




//设置AprilTag识别种子点 T_y为识别高度，T_color为赛道连通域的标志色
uint8 SetSeed_AprilTag(uint8 T_y,uint8 T_color)
{
    uint8 Contu_T_L=0;  //扫到T-B跳变点前的赛道连通域属色块个数
    //uint8 Contu_B_R=0;
    uint8 Jump_L=0;
    uint8 Jump_R=0;
    uint8 SeedFound=0;

    for(uint8 i=1;i<PIX_IMG_X;i++)  //从左到右扫到第一个T-B跳变点
    {
        if(copy_pix_img[T_y][i-1]==T_color && copy_pix_img[T_y][i]==T_color)
        {
            Contu_T_L++;
        }

        if(Contu_T_L>=2 && (copy_pix_img[T_y][i-1]==T_color && copy_pix_img[T_y][i]==Black))
        {
            Jump_L=i;
            break;
        }
    }

    for(uint8 i=Jump_L+1;i<PIX_IMG_X;i++)
    {
        if(copy_pix_img[T_y][i-1]==Black && copy_pix_img[T_y][i]==T_color)
        {
            Jump_R=i;
            SeedFound=1;
            break;
        }
    }

    if(SeedFound)
        return((uint8)((Jump_R+Jump_L)*0.5));
    else
        return 0;

}





void FloodSeedFill(uint8 x,uint8 y,uint8 Old_Color,uint8 New_Color)
{
    if((x>0 && x<PIX_IMG_X) && (y>0 && y<PIX_IMG_Y))  //检验范围
    {
        if(copy_pix_img[y][x]==Old_Color)
        {
            copy_pix_img[y][x]=New_Color;
            SumInCD++;
            for(uint8 i=0;i<4;i++)  //4连通域
            {
                FloodSeedFill(x+Direction_4[i].offset_x,y+Direction_4[i].offset_y,Old_Color,New_Color); //坑爹递归
            }
        }
    }
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

        if(Feature_Verify_Color(0,0,20,10,White,90))
        {
            flag_Garage_L=1;
            flag_Garage_R=0;    //因为没有每帧初始化，所以为了避免误置造成矛盾，故对两个标志位均置高
            return 1;
        }
        else if(Feature_Verify_Color(167,0,20,10,White,90))
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




//状态判断
uint8 Judge(void)
{
    //-------状态整理 <head>--------//
    
    if(flag_Y_Road_IN)
    {
        flag_Y_Road_IN--;
    }

    if(YRoadInCount)
    {
        YRoadInCount--;
    }

    if(RoundInCount)
    {
        RoundInCount--;
    }

    if(RoundOutCount)
    {
        RoundOutCount--;
    }

    if(CrossInCount)
    {
        CrossInCount--;
    }

    //-------状态整理 <bottom>--------//




    //------T路检测 <head>---------//
    if(Round_Status==0 && Feature_Verify_Color(0,44,187,5,Black,90))
    {
        flag_T_Road=1;
    }
    //------T路检测 <bottom>---------//



    //------车库检测 <head>---------//
    if(If_Garage())
    {
        return 1;
    }
    //------车库检测 <bottom>---------//




    //------三岔检测 <head>---------//
    //if(!YRoadInCount && !flag_Y_Road_IN && !Round_Status)
    if(!Round_Status)
    {
        uint8 SumMark_Lane_Head=0;  //图像最顶行的赛道标记个数  //这样规避了急弯、T路
        uint8 SumJumpMark_Lane_2_Black=0;   //从赛道连通域标记到黑块的跳变点的个数
        uint8 SumJumpBlack_2_Mark_Lane=0;   //从赛道连通域标记到黑块的跳变点的个数

        for(uint8 i=0;i<PIX_IMG_X;i++)
        {
            if(copy_pix_img[PIX_IMG_Y-1][i]==Mark_Lane)
            {
                SumMark_Lane_Head++;
            }
        }

        if(SumMark_Lane_Head)
        {
            for(uint8 j=PIX_IMG_Y-4;j<PIX_IMG_Y;j++)
            {
                uint8 JumpP1=0; //找到第一种跳变点标志 以规定跳变点的顺序 每行初始化

                for(uint8 i=1;i<PIX_IMG_X;i++)
                {
                    if(copy_pix_img[j][i-1]==Mark_Lane && copy_pix_img[j][i]==Black)    //跳变点
                    {
                        SumJumpMark_Lane_2_Black++;
                        JumpP1=1;
                    }

                    if(JumpP1 && copy_pix_img[j][i-1]==Black && copy_pix_img[j][i]==Mark_Lane)    //跳变点
                    {
                        SumJumpBlack_2_Mark_Lane++;
                    }
                }
            }
        }

        //temp=SumMark_Lane_Head;
        temp1=SumJumpMark_Lane_2_Black;
        temp2=SumJumpBlack_2_Mark_Lane;

        if(SumJumpMark_Lane_2_Black>=4 && SumJumpBlack_2_Mark_Lane>=4)   //扫了4行，每行每类起码1个跳变点 条件已放宽
        {
            flag_Y_Road=1;
            bb_time=50;
            //openmv确认三岔转向
            YRoadInCount=30;
            flag_Y_Road_IN=40;
        }
    }

    if(ad_value_all<=400)
    {
        flag_Y_Road_IN=40;
    }

    /*
    if(!YRoadInCount && !flag_Y_Road_IN && Feature_Verify_Color(74,44,40,5,Black,90)) 
    {
        if(Feature_Verify_Color(29,20,130,5,White,90) && Feature_Verify_Color(0,44,20,5,White,90) && Feature_Verify_Color(167,44,20,5,White,90))
        {
            flag_Y_Road=1;

            //bb_time=50;


                //openmv确认三岔转向

            //return 1;
            YRoadInCount=30;
            flag_Y_Road_IN=40;
        }
    }
    
   if(!YRoadInCount && !flag_Y_Road_IN && !Round_Status && EdgeNum<IMG_Y*0.6)
    {
        if(Feature_Verify_Color(5,5,40,5,White,87) && Feature_Verify_Color(142,5,40,5,White,87))
        {
            flag_Y_Road=1;
            flag_Y_Road_IN=40;
            bb_time=5;


        }
    }
    */
    //-------三岔检测 <bottom>--------//





    //-------十字检测 <head>--------//
    if(CrossInCount && Round_Status<=4)
    {
        Round_Status=0;
    }

    if(!flag_T_Road && (Round_Status<=5) && EdgeNum>=IMG_Y*0.6)
    {   
        if(Feature_Verify_Color(0,23,187,3,White,90))
        {
            flag_Cross=1;
            CrossInCount=15;
            return 1;
        }
        /*
        else if(ad_value1>=120 && ad_value6>=120)
        {
            flag_Cross=1;
            CrossInCount=15;
            return 1;
        }
        */
    }
    //-------十字检测 <bottom>--------//




    //------环岛检测 <head>---------//

    switch(Round_Status)
    {
        case 0:
            if(flag_Y_Road || Feature_Verify_Color(83,9,20,40,Black,10))
            {
                break;
            }
        
            if(!RoundOutCount && Feature_Verify_Color(10,10,50,8,White,90) && !Feature_Verify_Color(127,10,50,3,White,90))
            {
                Round_Status=1;

                flag_Normal_Lose_L=1; //避免打偏
            }
            else if(!RoundOutCount && Feature_Verify_Color(127,10,50,8,White,90) && !Feature_Verify_Color(10,10,50,3,White,90))
            {
                Round_Status=2;

                flag_Normal_Lose_R=1;
            }
        
            break;

        case 1:
            if(ad_value_all>Round_ad_limit && Feature_Verify_Color(0,10,20,10,Black,90))
            {
                if(ad_value_all<Round_ad_limit)
                {
                    Round_Status=0;
                    break;
                }

                Round_Status=3;
            }
            break;

        case 2:
            if(ad_value_all>Round_ad_limit && Feature_Verify_Color(167,10,20,10,Black,90))
            {
                if(ad_value_all<Round_ad_limit)
                {
                    Round_Status=0;
                    break;
                }

                Round_Status=4;
            }
            break;

        case 3:
            if(!RoundInCount && Feature_Verify_Color(0,10,50,10,White,90))
            {
                RoundInCount=25;
            }

            if(RoundInCount==1)
            {
                Round_Status=5;
            }

            flag_Normal_Lose_L=1;

            break;

        case 4:
            if(!RoundInCount && Feature_Verify_Color(137,10,50,10,White,90))
            {
                RoundInCount=25;
            }

            if(RoundInCount==1)
            {
                Round_Status=6;
            }

            flag_Normal_Lose_R=1;

            break;
        case 5:
            if(ad_value_all>Round_ad_limit-50)
            {
                Round_Status=7;
            }

            flag_Normal_Lose_L=1;   //否则在大环内容易晃
            break;

        case 6:
            if(ad_value_all>Round_ad_limit-50)
            {
                Round_Status=8;
            }
            
            flag_Normal_Lose_R=1;
            break;

        case 7:
            if(1)
            {
                Round_Status=9;

                flag_Normal_Lose_L=1;
            }
            break;

        case 8:
            if(1)
            {
                Round_Status=10;

                flag_Normal_Lose_R=1;
            }
            break;

        case 9:
            /*
            if(!RoundOutCount && Feature_Verify_Color(0,10,50,10,White)>=90)
            {
                Round_Status=10;

                RoundOutCount=40;
                flag_Normal_Lose_L=1;
            }
            */

            if(!RoundOutCount && Feature_Verify_Color(84,30,20,19,White,80))
            {
                Round_Status=11;

                RoundOutCount=50;
                flag_Normal_Lose_L=1;
            }
            break;

        case 10:
            /*
            if(!RoundOutCount && Feature_Verify_Color(137,10,50,10,White)>=90)
            {
                Round_Status=11;
                RoundOutCount=40;
                flag_Normal_Lose_R=1;
            }
            break;
            */

            if(!RoundOutCount && Feature_Verify_Color(84,30,20,19,White,80))
            {
                Round_Status=12;
                RoundOutCount=50;
                flag_Normal_Lose_R=1;
            }
            break;

        case 11:
            if(RoundOutCount==1)
            {
                Round_Status=0;
            }
            break;

        case 12:
            if(RoundOutCount==1)
            {
                Round_Status=0;
            }
            break;
    }

    if((flag_Y_Road || flag_Y_Road_IN) && Round_Status<3)    //防止三岔误识别为环岛
    {
        Round_Status=0;
    }

    if(RoundOutCount && Round_Status<=8)
    {
        Round_Status=0;
    }
    //------环岛检测 <bottom>---------//




    //------直路检测 <head>---------//
    if((!Round_Status || flag_Y_Road || flag_Y_Road_IN) && Feature_Verify_Color(84,40,20,8,White,90))
    {
        flag_Straight=1;
    }
    //------T路检测 <bottom>---------//



    //------AprilTag检测 <head>---------//
    if(!Round_Status && !flag_Y_Road)   //更改以排除互斥元素 例：if(!Round_Status)
    {
        if(sumincd.AprilTag>6 && sumincd.AprilTag<80)   //目前仅根据连通域内像素个数判断，因为扫线高度是固定的
        {
            flag_AprilTag=1;
            bb_time=20;
        }
    }
    /*
    uint8 AprilTag_Height_Limit=0;
    for(uint8 i=0;i<PIX_IMG_X;i++)  //在图像高度为十的位置水平扫描，避免连通域泄露
    {
        if(copy_pix_img[10][i]==Mark_AprilTag)
        {
            AprilTag_Height_Limit++;
        }
    }
    */
    /*
    Copy_pix_img(); //先复制图像
    FloodSeedFill(SetSeed_AprilTag(6),6,Black,Gray);    //设定种子并测试连通域

    if(SumInCD>=6 && SumInCD<=90)
    {
        flag_AprilTag=1;

        bb_time=20;
    }
    */
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
    Connect_pp(1,40,0,70,48);
    Connect_pp(0,148,0,118,48);
    /*
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
    */
}



//丢边补全
uint8 If_Lose_Edge(void)
{

switch(Round_Status)
{
    case 0:
        break;

    case 1:
        flag_Normal_Lose_L=1;
        break;

    case 2:
        flag_Normal_Lose_R=1;
        break;

    case 3:
        bb_time=5;
        if(RoundInCount>10)
        {
            Connect_pp(0,120,0,10,48);
            flag_Normal_Lose_L=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        }

        break;

    case 4:
        bb_time=5;

        if(RoundInCount>10)
        {
            Connect_pp(1,68,0,178,48);
            flag_Normal_Lose_R=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        }

        break;

    case 5:
        Connect_pp(0,120,0,10,48);
        flag_Normal_Lose_L=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 6:
        Connect_pp(1,68,0,178,48);
        flag_Normal_Lose_R=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 7:
        Connect_pp(0,120,0,10,48);
        flag_Normal_Lose_L=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 8:
        Connect_pp(1,68,0,178,48);
        flag_Normal_Lose_R=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 9:
        Connect_pp(0,120,0,10,48);
        flag_Normal_Lose_L=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 10:
        Connect_pp(1,68,0,178,48);
        flag_Normal_Lose_R=1;   //ATTENTION 另外此处修改了电感偏差，参见aicar_error.c
        break;

    case 11:
        flag_Normal_Lose_L=1;
        break;

    case 12:
        flag_Normal_Lose_R=1;
        break;
}

/*
    if(flag_Round_out_L && RoundOutCount>80)
    {
        Connect_pp(0,120,0,20,40);
        flag_Normal_Lose_L=1;
    }
    else if(flag_Round_out_R && RoundOutCount>80)
    {
        Connect_pp(1,68,0,168,40);
        flag_Normal_Lose_R=1;
    }



    if(flag_Round_in_L && RoundInCount>0)
    {
        Connect_pp(0,120,0,30,40);
        flag_Normal_Lose_L=1;
    }
    else if(flag_Round_in_R && RoundInCount>0)
    {
        Connect_pp(1,68,0,158,40);
        flag_Normal_Lose_R=1;
    }
*/

/*
    if(flag_Round_in_L && flag_Normal_Lose_L)   //避免出环转错方向
    {
        flag_Normal_Lose_R=1;
        flag_Normal_Lose_L=0;
    }
    else if(flag_Round_in_R && flag_Normal_Lose_R)
    {
        flag_Normal_Lose_L=1;
        flag_Normal_Lose_R=0;
    }
*/

    if(flag_Y_Road)
    {
        //openart通信
        bb_time=10;
        //sancha_stop();
        YRoadInCount=30;
    }

    if(flag_T_Road)
    {
        uint16 Height_L=0;
        uint16 Height_R=0;
        uint8 Height=0;
        for(uint8 i=0;i<IMG_X/2;i++)
        {   
            Height=0;
            for(uint8 j=0;j<IMG_Y-1;j++)
            {
                if(img[j][i]==Black || j==IMG_Y-2)
                {
                    Height=j;
                    break;
                }
            }
            Height_L+=Height;
        }

        for(uint8 i=IMG_X/2;i<IMG_X;i++)
        {
            Height=0;
            for(uint8 j=0;j<IMG_Y-1;j++)
            {
                if(img[j][i]==Black || j==IMG_Y-2)
                {
                    Height=j;
                    break;
                }
            }
            Height_R+=Height;
        }

        if(Height_L>Height_R)
        {
            Connect_pp(0,120,0,20,49);
            flag_Normal_Lose_L=1;
        }
        else if(Height_L<Height_R)
        {
            Connect_pp(1,68,0,168,49);
            flag_Normal_Lose_R=1;
        }
    }

    if(CrossInCount)  //十字
    {
        Damn_Lose_Edge_all();   //双侧丢边函数
        return 1;
    }
    else if(flag_Normal_Lose_L)  //单左丢边
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Lx=edge[i].Rx-Width_Cali(i);    //删边补全  （直接覆写左边界数据
        }
        return 1;
    }
    else if(flag_Normal_Lose_R)  //单右丢边
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Rx=edge[i].Lx+Width_Cali(i);
        }
        return 1;
    }

    return 0;
}




//计算中点&更新为折线(减少意外出错)
void Set_Mid(void)
{
    for(uint8 i=0;i<=EdgeNum;i++)   //计算中点
    {

        mid[i]=(edge[i].Lx+edge[i].Rx)/2;

        if(i>=5)    //待中线稳定后，对超过容差的相邻中线进行判断
        {
            if(mid[i]-mid[i-1]>=20 || mid[i]-mid[i-1]<=-20)
            {
                EdgeNum=i-1;  //结束中点计算
            }
        }
        
    }

    /*
    for(uint8 i=1;i<(EdgeNum/MID_RESOLUTION)-1;i++)   //中线拟合成直线
    {
        Mid_Connect(mid,(i+1)*MID_RESOLUTION,i*MID_RESOLUTION);
    }
    */

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

    init();
    Y_Change();
    zoomin(0.5,0);
    Copy_pix_img();

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
    

    //-------------------连通域设置部分 <head>---------------//
    if(SetSeed_Lane(MidStart,EdgeNum))
    {
        FloodSeedFill(MidStart/3,EdgeNum/4,White,Mark_Lane);
        sumincd.lane=SumInCD;
        SumInCD=0;
        FloodSeedFill(SetSeed_AprilTag(5,Mark_Lane),5,Black,Mark_AprilTag);
        sumincd.AprilTag=SumInCD;
        SumInCD=0;
    }

    //-------------------连通域设置部分 <bottom>-------------//


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
    if(flag_Garage_L)
    {
        aicar_left_garage_in();
    }
    else if(flag_Garage_R)
    {
        aicar_right_garage_in();
    }
    //-------------------结束部分 <bottom>-------------//
}

#include "fd_GetMid.h"

//data
uint8 img[IMG_Y][IMG_X];
EDGE edge[EDGE_MAX];    //�߽�ṹ������ �߽���ֵΪ0��ʱ��Ϊ����
ANGLE angle[EDGE_MAX];  //���ؽǶ�����
uint8 EdgeNum;    //������
uint8 EdgeLosePos[EDGE_MAX];  //����λ�� (Y��           ����֡��ʼ����
uint8 EdgeLoseNum;   //������                           ����֡��ʼ����
int16 mid[EDGE_MAX];    //TODO ȷ���������������С      ����֡��ʼ����
uint8 MidStart=IMG_X/2;   //�ױ�������ʼ�������
uint16 Round_ad_limit=600;  //�뻷ad��ֵ
uint16 Croess_ad_limit=120;
uint32 RunningCount=0;
uint16 RoundInCount=0; //�뻷����
uint8 RoundOutCount=0;
uint8 YRoadInCount=0;
uint8 CrossInCount=0;

//flag
//------------------------����״̬��־,��ɨ�ߺ�������------------------------//
uint8 flag_MidStart_in_Black=0;   //Hor_Search �����и�����ʼ���ں�����    ����֡��ʼ����
uint8 flag_Ver_Search_Finish=0;   //��ֱɨ�����                          ����֡��ʼ����

//-------------------������־,ԭ���Ͻ���ɨ�ߺ�����״̬������------------------//
uint8 flag_LoseEdge_part_L=0; //�󲿷ֶ���     ����֡��ʼ����
uint8 flag_LoseEdge_part_R=0; //              ����֡��ʼ����
uint8 flag_LoseEdge_all_L=0;  //              ����֡��ʼ����
uint8 flag_LoseEdge_all_R=0;  //              ����֡��ʼ����
uint8 flag_Straight_L=0;  //��ֱ��    ���ֱ���ȼ������򵥵Ķ�            ����֡��ʼ����
uint8 flag_Straight_R=0;  //��ֱ��                                         ����֡��ʼ����

//------------------�߼���־,ԭ������״̬��ȷ�����ߺ�������-------------------//
uint8 flag_Straight=0;
uint8 flag_T_Road=0;   //T��·����ָ��ϼ���ȫ�ڣ����³��ִ�������ඪ�ߵ�״��
uint8 flag_Y_Road=0;   //����
uint8 flag_Y_Road_IN=0; //������
uint8 Y_Road_Status=1;  //������    //ATTENTION ��openart���� 0��1��
uint8 flag_Cross=0;    //ʮ��                 ����֡��ʼ����
uint8 Round_Status=0; //0ɶ��û�� 1��һ�ο������� 3���������ҵ�д�����ֵ 5�ڶ��ο������� 7���� 9���� (ͬ����Ϊż)
uint8 flag_Normal_Lose_L=0;   //һ�㶪���                 ����֡��ʼ����
uint8 flag_Normal_Lose_R=0;   //һ�㶪�ұ�                 ����֡��ʼ����
uint8 flag_Garage_L=0;        //���������
uint8 flag_Garage_R=0;        //�������Ҳ�
uint8 flag_AprilTag_ARM=0;    //AprilTagԤλ ��λ�����AprilTag�жϺ������жϴ��� ����֡��ʼ����
uint8 flag_AprilTag=0;        //AprilTag����                                     ����֡��ʼ����

//--------------------------------����/������־------------------------------//
uint8 flag_Indct_Help_Me;   //�������ѭ��  ������ǵø�λ               ����֡��ʼ����



//******************************* Base Function *********************************/
void init(void)
{
    //****��֡��־λ��ʼ��****
    flag_LoseEdge_part_L=0;
    flag_LoseEdge_part_R=0;
    flag_LoseEdge_all_L=0;
    flag_LoseEdge_all_R=0;
    flag_Y_Road=0;
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
    flag_Straight=0;
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



//���ٿ��� ����ѧ���ǳ�������
float Cp_sqrt(float number)
{
	long i;
	float x, y;
	const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = *(long*)&y;	//	ȡ
	i = 0x5f3759df - (i >> 1);  //�����
	//i  = 0x5f375a86 - ( i >> 1 );  //Lomont
	y = *(float*)&i;
	y = y * (f - (x * y * y));
	y = y * (f - (x * y * y));
	return number * y;
}



//��ȡ�Ƕ� ���������н�
float Get_Angle(uint8 p1_y,uint8 p2_y,uint8 Target_y,uint8 l_or_r)
{
    float cos=0;
    float FA_x=0;   //����FA
    float FA_y=0;
    float FB_x=0;   //����FB
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
    
    cos=(FA_x*FB_x + FA_y*FB_y); //����
    cos/=Cp_sqrt(FA_x*FA_x + FA_y*FA_y)*Cp_sqrt(FB_x*FB_x + FB_y*FB_y); //��ĸ

    return (acos(cos)*57.295); //ת���ɽǶ������ ������ <math.h>
}




void Del_Edge_Data(uint8 p1_y,uint8 p2_y)
{
    for(uint8 i=p1_y;i<=p2_y;i++)
    {
        edge[i].Lx=0;
        edge[i].Rx=0;
    }
}




//ʹ��ǰ���������ߵ�б������Ŀ���
uint8 Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r) //ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ�
{
    float Slope=0;  //б�� ��ʵ��cot
    
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



//�������� ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ� //ATTENTION p1_yС��p2_y
uint8 Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y)
{
    float Slope=0;   //б�� ��ʵ��cot
    if(l_or_r)
    {
        if((p1_y-p2_y) != 0)  //��ֱ��ʱ��Ͳ�������
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
        if((p1_y-p2_y) != 0)  //��ֱ��ʱ��Ͳ�������
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

    if(l_or_r)
    {
        Slope=(p2_x-p1_x)/(p2_y-p1_y);

        for(uint8 i=p1_y;i<=p2_y;i++)
        {
            edge[i].Lx=p1_x+i*Slope;
        }

        return 1;
    }
    else
    {
        Slope=(p2_x-p1_x)/(p2_y-p1_y);

        for(uint8 i=p1_y;i<=p2_y;i++)
        {
            edge[i].Rx=p1_x+i*Slope;
        }

        return 1;
    }
}



//�������� ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ�    //ATTENTION p1_yС��p2_y
uint8 Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y) 
{
    float Slope=0;   //б�� ��ʵ��cot
    if((Target[p1_y]-Target[p2_y]) != 0)  //��ֱ��ʱ��Ͳ�������
    {
        Slope=(p1_y-p2_y)/(Target[p1_y]-Target[p2_y]);

        for(int i=1;i<=p2_y-p1_y;i++)
        {
            Target[p1_y+i]=(uint8)(i*Slope+Target[p1_y]);
        }
    }
    return 1;
}


//ˮƽɨ�ߣ���ײ㣩 //ATTENTION yΪ������Ƶ׿�ĸ߶� yΪ�ڲ��ݹ�(����)ʹ�õĲ������ⲿ��������ֵ
uint8 Hor_Search_Base(uint8 MidStart,uint8 y)
{
    flag_MidStart_in_Black=0;   //��־λ����

    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //��Χ��֤
    {

        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //����������е��ں�����
    {


        uint8 Sum_of_white_zone=0;  //��ǰ�а������� Ĭ��������� �޸ĸ�����ע��ͬʱ�޸�temp_edge�����Ԫ�ظ���,�Լ�������һ�Ѷ�Ҫ��
        EDGE temp_edge;  //��ʱ�߽��

        temp_edge.Lx=0;  
        temp_edge.Rx=IMG_X-1;

        for(uint8 i=MidStart;i>=X_LIMIT;i--)    //��������
        {
            if(Sum_of_white_zone!=1 &&img[y+Y_LIMIT][i]==White)    //ɨ�������Ͳ����޸��ұ���
            {
                temp_edge.Rx=i;
                Sum_of_white_zone=1;    //��־�Ѿ������ҵ�����
            }

            if(Sum_of_white_zone==1 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge.Lx=i;
            }
        }

        for(uint8 i=MidStart;i<IMG_X-X_LIMIT;i++)    //��������
        {
            if(Sum_of_white_zone!=2 && img[y+Y_LIMIT][i]==White)    //ɨ�������Ͳ����޸������
            {
                temp_edge.Lx=i;
                Sum_of_white_zone=2;    //��־�Ѿ������ҵ�����
            }

            if(Sum_of_white_zone==2 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge.Rx=i;
            }
        }

        if(!Sum_of_white_zone)  //û����
        {
            flag_Indct_Help_Me=1;   //�����л������ѭ��

            return 0;
        }


        MidStart=(temp_edge.Rx+temp_edge.Lx)/2;

    }

    for(uint8 i=MidStart;i>=X_LIMIT;i--)   //��������
    {
        if(img[y+Y_LIMIT][i]==Black)  //TODO �Ľ��ж����ķ���
        {
            edge[y].Lx=i;                         //ע�⣡����ֱ��ΪĬ��ֵ0 �������Ҳ�Ǹ���0�жϱ����Ƿ�Ϊ���������
            break;
        }

        if(i==X_LIMIT)  //����Ե��δ�ҵ����� �ж϶���
        {
            flag_LoseEdge_part_L++;
        }
    }

    for(uint8 i=MidStart;i<=IMG_X-X_LIMIT;i++)   //��������
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

    if(edge[y].Lx==0)   //�ײ㶪�� �Ǿ�����ͼ����ش�����
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



//ˮƽɨ�ߣ����У� //ATTENTION yΪ������Ƶ׿�ĸ߶�
uint8 Hor_Search(uint8 MidStart,uint8 y)
{
    flag_MidStart_in_Black=0;   //��־λ����

    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //��Χ��֤
    {
        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //����������е��ں�����
    {
        flag_MidStart_in_Black=1;
        return 0;
    }

    for(uint8 i=MidStart;i>=X_LIMIT;i--)   //��������
    {
        if(img[y+Y_LIMIT][i]==Black) 
        {
            edge[y].Lx=i;                         //TODO ȷ����ע�͵���ȷ��-> ע�⣡����ֱ��ΪĬ��ֵ0 �������Ҳ�Ǹ���0�жϱ����Ƿ�Ϊ���������
            break;
        }

        if(i==X_LIMIT)  //����Ե��δ�ҵ����� �ж϶���
        {
            flag_LoseEdge_part_L++;
        }
    }

    for(uint8 i=MidStart;i<=IMG_X-X_LIMIT;i++)   //��������
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

    if(edge[y].Rx-edge[y].Lx<=4)    //�����ǰ�����
    {
        EdgeNum=y;  //�������߶ȣ���������߼�ƫ�����Ӱ��
    }

    if(y>3) // �������ȶ���
    {
        if(edge[y].Lx-edge[y-2].Lx>=10 || edge[y].Lx-edge[y-2].Lx<=-10) //ATTENTION EdgeLosePos���ܻᴢ����ͬ�ĸ߶ȣ�˫�ඪ��ʱ��
        {
            edge[y].Lx=0;   //ͻ����������
            EdgeLosePos[EdgeLoseNum++]=y;    //���涪����Ϣ
        }

        if(edge[y].Rx-edge[y-2].Rx>=10 || edge[y].Rx-edge[y-2].Rx<=-10)
        {
            edge[y].Rx=0;   //ͻ����������
            EdgeLosePos[EdgeLoseNum++]=y;    //���涪����Ϣ
        }
    }

    if(y)   //������ױ�
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

    return ((edge[y].Rx-edge[y].Lx)/2);
}



//******************************* Base Function *********************************/


//*******************************************************************************/

 //��ʼ��ֱɨ��
uint8 Ver_Search(uint8 MidStart)
{
    EdgeNum=0;
    flag_Ver_Search_Finish=0;

    for(uint8 i=Y_LIMIT;i<=IMG_Y-Y_LIMIT;i++)
    {
        EdgeNum=i;  //�趨���߸߶� ��ɨ�����߶ȣ� EdgeNum ����Ϊ���鳤�� ��Y_LIMIT��ʼ
        if(img[i][MidStart]==Black)
        {
            flag_Ver_Search_Finish=1;
            break;
        }
    }

    return(EdgeNum);
}




//��ֱɨ�� ͨ�ô�ֱɨ�� ����ı�EdgeNum��ֵ   *ע�⣡ ��������Ǵ���ױ߿�ʼɨ��� //������Ϊɶ��д��һ�������������
uint8 Uni_Ver_Search(uint8 MidStart)
{
    uint8 IN_EdgeNum=0;

    for(uint8 i=0;i<=IMG_Y-Y_LIMIT;i++)
    {
        IN_EdgeNum=i;  //�趨���߸߶� ��ɨ�����߶ȣ� EdgeNum ����Ϊ���鳤�� ��Y_LIMIT��ʼ
        if(img[i][MidStart]==Black)
        {
            flag_Ver_Search_Finish=1;
            break;
        }
    }

    IN_EdgeNum-=Y_LIMIT*2;  //�ӵױ߿�ʼɨ�� ��ȥ˫���ı�Ե�߶�

    return(IN_EdgeNum);
}





void If_Straight(void)
{
    uint8 SearchTimesMax = EdgeNum - CUR_RESOLUTION; //�����������

    flag_Straight_L = 1; //�ȼ���Ϊֱ��
    flag_Straight_R = 1;

    for (uint8 i = 0; i < SearchTimesMax; i++) //ATTENTION ���ַ����޷����㵽����º����CUR_RESOLUTION/2�����صĽǶ�
    {
        angle[i].L = Get_Angle(i, i + CUR_RESOLUTION, i + CUR_RESOLUTION / 2, 1);
        angle[i].R = Get_Angle(i, i + CUR_RESOLUTION, i + CUR_RESOLUTION / 2, 0);
    }

    for (uint8 i = 0; i < SearchTimesMax; i++)
    {
        if (angle[i].L <= 160) //��160��Ϊ�� 180�Ǳ�׼ֱ��
        {
            flag_Straight_L = 0;
        }

        if (angle[i].R <= 160)
        {
            flag_Straight_R = 0;
        }
    }
}





//�����ȽϺ����������������ͼ���Ӧλ�ý��бȽϣ��������ƶ�(0~100) //ATTENTION T_x T_yΪ���½�����,����ע���������鲻ҪС��dx*dy
uint8 Feature_Verify(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 *feature)    
{
    float rate=0;

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //��Χ���
    {
        return(101);
    }

    for(uint8 i=0;i<dy;i++)
    {
        for(uint8 j=0;j<dx;j++)
        {
            if(img[T_y+i][T_x+j]==feature[i*dx+j])    //����������/ͼ��Ƚ�
            {
                rate++;
                //img[T_y+i][T_x+j]=Gray;
            }
        }
    }

    rate=(rate/(dx*dy))*100;

    return((uint8)rate);
}




//�����ȽϺ���(��ɫ),ɫ�ʷ�Χ0-255,expect_rate������0~100
uint8 Feature_Verify_Color(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 color,float expect_rate)    
{
    uint16 rate=0;
    uint16 unexpect_rate=((100-expect_rate)/100)*(dx*dy);

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //��Χ���
    {
        return(101);
    }

    for(uint8 i=0;i<dy;i++)
    {
        for(uint8 j=0;j<dx;j++)
        {
            if(img[T_y+i][T_x+j]!=color)    //����������/ͼ��Ƚ�
            {
                rate++;

                if(rate-unexpect_rate>0)  //����ģ��ƥ�䣬����������������ʱ���˳�
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
   return 1;    //���е��˴�ʱ˵���ѷ���Ҫ��
}




//�����ȽϺ���(���򣩣�thicknessΪ�����ȣ������������ͼ���Ӧλ�ý��бȽϣ��������ƶ�(0~100) 
uint8 Feature_Verify_Box(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 thickness,uint8 white_or_black)
{
    float rate=0;
    uint8 feature=white_or_black?White:Black; //ȷ����Ƚϵ���ɫ

    if(T_y+dy>=IMG_Y || T_x+dx>=IMG_X)  //��Χ���
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



//�жϰ����ߣ����⣩
uint8 If_Garage(void)
{
    uint8 StripeNum=0;  //������

    for(uint8 i=0;i<IMG_X-1;i++)
    {
        if(img[3][i]==White && img[3][i+1]==Black)
        {
            StripeNum++;
        }
    }

    if(StripeNum>=6)    //�����������趨ֵ
    {
        #ifdef IMG_DEBUG
        for(uint8 i=0;i<IMG_X;i++)  //�滭�����Ա����
        {
            img[3][i]=Gray;
        }
        #endif

        if(Feature_Verify_Color(0,0,20,10,White,90))
        {
            flag_Garage_L=1;
            flag_Garage_R=0;    //��Ϊû��ÿ֡��ʼ��������Ϊ�˱����������ì�ܣ��ʶ�������־λ���ø�
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




//״̬�ж�
uint8 Judge(void)
{
    //-------״̬���� <head>--------//

    if(flag_Y_Road_IN)
    {
        flag_Y_Road_IN--;
        flag_Y_Road=0;
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

    //-------״̬���� <bottom>--------//




    //------T·��� <head>---------//
    if(Round_Status==0 && Feature_Verify_Color(0,44,187,5,Black,90))
    {
        flag_T_Road=1;
    }
    //------T·��� <bottom>---------//



    //------������ <head>---------//
    if(If_Garage())
    {
        return 1;
    }
    //------������ <bottom>---------//




    //------������ <head>---------//
    if(Feature_Verify_Color(74,44,40,5,Black,90)) 
    {
        if(Feature_Verify_Color(29,20,130,5,White,90) && Feature_Verify_Color(0,44,20,5,White,90) && Feature_Verify_Color(167,44,20,5,White,90))
        {
            flag_Y_Road=1;

            //bb_time=50;

            /*
                    openmvȷ������ת��
            */

            //return 1;
            YRoadInCount=5;
            flag_Y_Road_IN=40;
        }
    }

    if(ad_value_all<=300)
    {
        flag_Y_Road_IN=40;
    }
    //-------������ <bottom>--------//





    //-------˫�ඪ�� <head>--------//
    if(CrossInCount && Round_Status<=4)
    {
        Round_Status=0;
    }

    if(!flag_T_Road && (Round_Status<=5))
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
    //-------˫�ඪ�� <bottom>--------//




    //------������� <head>---------//

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

                flag_Normal_Lose_L=1; //�����ƫ
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
                Round_Status=3;
            }
            break;

        case 2:
            if(ad_value_all>Round_ad_limit && Feature_Verify_Color(167,10,20,10,Black,90))
            {
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
            if(Feature_Verify_Color(167,39,20,10,White,90))
            {
                Round_Status=7;

                flag_Normal_Lose_L=1;   //�����ڴ������׻�
            }
            break;

        case 6:
            if(Feature_Verify_Color(0,39,20,10,White,90))
            {
                Round_Status=8;

                flag_Normal_Lose_R=1;
            }
            break;

        case 7:
            if(ad_value_all>Round_ad_limit)
            {
                Round_Status=9;

                flag_Normal_Lose_L=1;
            }
            break;

        case 8:
            if(ad_value_all>Round_ad_limit)
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

            if(!RoundOutCount)
            {
                Round_Status=11;

                RoundOutCount=40;
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

            if(!RoundOutCount)
            {
                Round_Status=12;
                RoundOutCount=40;
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

    if((flag_Y_Road || flag_Y_Road_IN) && flag_Y_Road<3)    //��ֹ������ʶ��Ϊ����
    {
        Round_Status=0;
    }
    //------������� <bottom>---------//




    //------ֱ·��� <head>---------//
    if((!Round_Status || flag_Y_Road || flag_Y_Road_IN) && Feature_Verify_Color(84,40,20,8,White,90))
    {
        flag_Straight=1;
    }
    //------T·��� <bottom>---------//



    //------AprilTag��� <head>---------//
    if(Feature_Verify_Box(MidStart-30,10,60,20,2,1)>=80 && Feature_Verify_Color(MidStart-28,5,56,13,Black,20))
    {
        //bb_time=40;

        return 1;
    }
    //------AprilTag��� <bottom>---------//




    //-------���ඪ�� <head>--------//
    if(flag_LoseEdge_part_L!=0 && flag_LoseEdge_part_R==0) //���󶪱�
    {
        flag_Normal_Lose_L=1;   //��ͨ���󶪱�
        return 1;
    }
    else if(flag_LoseEdge_part_L==0 && flag_LoseEdge_part_R!=0) //���Ҷ���
    {
        flag_Normal_Lose_R=1;   //��ͨ���Ҷ���
        return 1;
    }

    //-------���ඪ�� <bottom>--------//


    return 0;
}



//���ݸ߶ȼ����������
inline uint8 Width_Cali(uint8 y)
{
    if(0)   //��ʱ��Ϊ�궨ֵ����
        return((edge[0].Rx-edge[0].Lx)-(uint8)(WIDTH_K*y)); //ʹ����ײ��ȼ���
    else
        return(WIDTH_BASE-(uint8)(WIDTH_K*y)); //ʹ�ñ궨ֵ����
}



//˫�ඪ��
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
        if(edge[i].Lx!=0 && edge[i].Rx!=0)  //�ж��Ƿ���ڱ���
        {
            flag_Edge_Start_Exist=1;
            EdgeStartPos=i;
            break;
        }
    }

    for(uint8 i=EdgeNum;i>0;i--)
    {
        if(edge[i].Lx!=0 && edge[i].Rx!=0)  //�ж��Ƿ���ڱ���
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



//���߲�ȫ
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
            flag_Normal_Lose_L=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        }

        break;

    case 4:
        bb_time=5;

        if(RoundInCount>10)
        {
            Connect_pp(1,68,0,178,48);
            flag_Normal_Lose_R=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        }

        break;

    case 5:

        break;

    case 6:
        
        break;

    case 7:
        Connect_pp(0,120,0,10,48);
        flag_Normal_Lose_L=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        break;

    case 8:
        Connect_pp(1,68,0,178,48);
        flag_Normal_Lose_R=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        break;

    case 9:
        break;

    case 10:
        break;

    case 11:
        if(RoundOutCount>30)
        {
            Connect_pp(0,120,0,10,48);
            flag_Normal_Lose_L=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        }
        break;

    case 12:
        if(RoundOutCount>30)
        {
            Connect_pp(1,68,0,178,48);
            flag_Normal_Lose_R=1;   //ATTENTION ����˴��޸��˵��ƫ��μ�aicar_error.c
        }
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
    if(flag_Round_in_L && flag_Normal_Lose_L)   //�������ת����
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
        if(Y_Road_Status && YRoadInCount)
        {

        }
        else if(!Y_Road_Status && YRoadInCount)
        {

        }
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

    if(CrossInCount)  //ʮ��
    {
        Damn_Lose_Edge_all();   //˫�ඪ�ߺ���
        return 1;
    }
    else if(flag_Normal_Lose_L)  //���󶪱�
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Lx=edge[i].Rx-Width_Cali(i);    //ɾ�߲�ȫ  ��ֱ�Ӹ�д��߽�����
        }
        return 1;
    }
    else if(flag_Normal_Lose_R)  //���Ҷ���
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Rx=edge[i].Lx+Width_Cali(i);
        }
        return 1;
    }

    return 0;
}




//�����е�&����Ϊ����(�����������)
void Set_Mid(void)
{
    for(uint8 i=0;i<=EdgeNum;i++)   //�����е�
    {
        if((edge[i].Lx * edge[i].Rx) != 0)  //����δ���ߵ����
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
        
        if(i>=5)    //�������ȶ��󣬶Գ����ݲ���������߽����ж�
        {
            if(mid[i]-mid[i-1]>=20 || mid[i]-mid[i-1]<=-20)
            {
                EdgeNum=i-1;  //�����е����
            }
        }
        
    }

    
    for(uint8 i=1;i<(EdgeNum/MID_RESOLUTION)-1;i++)   //������ϳ�ֱ��
    {
        Mid_Connect(mid,(i+1)*MID_RESOLUTION,i*MID_RESOLUTION);
    }
    

}



//�򵥼�����
void Simple_Err_Check(void) 
{

}




//�滭����
void Print_Mid(void)
{
    for(uint8 i=Y_LIMIT; i<=EdgeNum+Y_LIMIT; i++)
    {
        img[i][edge[i-Y_LIMIT].Lx]=Gray;
        img[i][edge[i-Y_LIMIT].Rx]=Gray;

        if(mid[i]<IMG_X && mid[i]>0)    //��Ҳ��֪��Ϊʲô��ʱҪд�����ֵ�����
            img[i][mid[i-Y_LIMIT]]=Gray;
    }

}



//�ӵ���ɨ��������
void Search(void)
{

    init();
    Y_Change();

    //-------------------ɨ�߲��� <head>--------------//
    Hor_Search_Base(MidStart,0);   //��һ����ȷ��ɨ���׼����
    MidStart=(edge[0].Lx+edge[0].Rx)/2;

    Ver_Search(MidStart);   //��ֱɨ�� ����ɨ�����߶� EdgeNum

    for(uint8 i=0;i<=EdgeNum;i++)   //ˮƽɨ��ѭ��
    {
        Hor_Search(MidStart,i);
                                //TODO ȱһ���Զ��߸������ж����� flag_LoseEdge_all_X��λ
    }

    //If_Straight();  //ȷ��ֱ��

    
    //-------------------ɨ�߲��� <bottom>-------------//


    //-------------------�жϲ��� <head>---------------//
    Judge();
    //-------------------�жϲ��� <bottom>-------------//


    //-------------------���߲��� <head>---------------//
    If_Lose_Edge(); 

    Set_Mid();
    //-------------------���߲��� <bottom>-------------//


    //-------------------�滭���� <head>---------------//
    Print_Mid();
    //-------------------�滭���� <bottom>-------------//

    //-------------------�������� <head>---------------//
    if(flag_Garage_L)
    {
        aicar_left_garage_in();
    }
    else if(flag_Garage_R)
    {
        aicar_right_garage_in();
    }
    //-------------------�������� <bottom>-------------//
}

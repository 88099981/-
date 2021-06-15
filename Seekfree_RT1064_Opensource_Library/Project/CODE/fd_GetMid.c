#include "fd_GetMid.h"

//data
uint8 img[IMG_Y][IMG_X];
EDGE edge[EDGE_MAX];    //�߽�ṹ������ �߽���ֵΪ0��ʱ��Ϊ����
ANGLE angle[EDGE_MAX];  //���ؽǶ�����
uint8 EdgeNum;    //������
uint8 EdgeLosePos[EDGE_MAX];  //����λ�� (Y��           ����֡��ʼ����
uint8 EdgeLoseNum;   //������                           ����֡��ʼ����
int16 mid[EDGE_MAX];    //TODO ȷ���������������С      ����֡��ʼ����
int8 CamError;

//flag
//------------------------����״̬��־,��ɨ�ߺ�������------------------------//
uint8 flag_MidStart_in_Black;   //Hor_Search �����и�����ʼ���ں�����    ����֡��ʼ����
uint8 flag_Ver_Search_Finish;   //��ֱɨ�����                          ����֡��ʼ����

//-------------------������־,ԭ���Ͻ���ɨ�ߺ�����״̬������------------------//
uint8 flag_LoseEdge_part_L; //�󲿷ֶ���     ����֡��ʼ����
uint8 flag_LoseEdge_part_R; //              ����֡��ʼ����
uint8 flag_LoseEdge_all_L;  //              ����֡��ʼ����
uint8 flag_LoseEdge_all_R;  //              ����֡��ʼ����
uint8 flag_Straight_L;  //��ֱ��    ���ֱ���ȼ������򵥵Ķ�            ����֡��ʼ����
uint8 flag_Straight_R;  //��ֱ��                                         ����֡��ʼ����

//------------------�߼���־,ԭ������״̬��ȷ�����ߺ�������-------------------//
uint8 flag_Cross;    //ʮ��                 ����֡��ʼ����
uint8 flag_Round_ARM_L;  //�󻷵�Ԥλ
uint8 flag_Round_ARM_R;  //�һ���Ԥλ
uint8 flag_Is_This_Round;   //�������߱�־  ѯ�ʵ��(������ǵø�λ)    ��Ϊʲô���������溣���أ�
uint8 flag_Round_in_L;   //�󻷵����� ���뻷��ʼ��λ
uint8 flag_Round_in_R;   //�һ�������
uint8 flag_Normal_Lose_L;   //һ�㶪���                 ����֡��ʼ����
uint8 flag_Normal_Lose_R;   //һ�㶪�ұ�                 ����֡��ʼ����
uint8 flag_AprilTag_ARM;    //AprilTagԤλ ��λ�����AprilTag�жϺ������жϴ��� ����֡��ʼ����
uint8 flag_AprilTag;        //AprilTag����                                     ����֡��ʼ����

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
        edge[i].Rx=camera_raw_data_X-1;
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




float Cp_sqrt(float number)	//���ٿ��� ����ѧ���ǳ�������
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




float Get_Angle(uint8 p1_y,uint8 p2_y,uint8 Target_y,uint8 l_or_r)//��ȡ�Ƕ� ���������н�
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

    if(FA_x&FB_x&FA_y&FB_y)
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
void Set_Edge_BySlope(uint8 p1_y,uint8 p2_y,uint8 target_p_y,uint8 l_or_r) //ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ�
{
    float Slope=0;  //б�� ��ʵ��cot
    
    if(l_or_r)
    {
        Slope=(edge[p1_y].Lx - edge[p2_y].Lx)/(p1_y - p2_y);
        edge[target_p_y].Lx = edge[p1_y].Lx + Slope*target_p_y;
    }
    else
    {
        Slope=(edge[p1_y].Rx - edge[p2_y].Rx)/(p1_y - p2_y);
        edge[target_p_y].Rx = edge[p1_y].Lx + Slope*target_p_y;
    }
}




void Connect(EDGE Target[],uint8 l_or_r,uint8 p1_y,uint8 p2_y) //ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ�    ע��p1_yС��p2_y
{
    float Slope=0;   //б�� ��ʵ��cot
    if(l_or_r)
    {
        if((Target[p1_y].Lx-Target[p2_y].Lx) != 0)  //��ֱ��ʱ��Ͳ�������
        {
            Slope=(p1_y-p2_y)/(Target[p1_y].Lx-Target[p2_y].Lx);

            for(int i=1;i<=p2_y-p1_y;i++)
            {
                Target[p1_y+i].Lx=i*Slope+Target[p1_y].Lx;
            }
        }
    }
    else
    {
        if((Target[p1_y].Rx-Target[p2_y].Rx) != 0)  //��ֱ��ʱ��Ͳ�������
        {
            Slope=(p1_y-p2_y)/(Target[p1_y].Rx-Target[p2_y].Rx);

            for(int i=1;i<=p2_y-p1_y;i++)
            {
                Target[p1_y+i].Rx=i*Slope+Target[p1_y].Rx;
            }
        }
    }
}





void Mid_Connect(int16 Target[],uint8 p1_y,uint8 p2_y) //ͨ����Ϊ�߶Ⱥͱ��ص��Ƕ�Ӧ�ģ�������������������ʾ��Ӧ�ĵ�    ע��p1_yС��p2_y
{                                                      //���� ��ΪmidΪint16����
    float Slope=0;   //б�� ��ʵ��cot
    if((Target[p1_y]-Target[p2_y]) != 0)  //��ֱ��ʱ��Ͳ�������
    {
        Slope=(p1_y-p2_y)/(Target[p1_y]-Target[p2_y]);

        for(int i=1;i<=p2_y-p1_y;i++)
        {
            Target[p1_y+i]=i*Slope+Target[p1_y];
        }
    }
}



uint8 Hor_Search_Base(uint8 MidStart,uint8 y)    //ˮƽɨ�ߣ���ײ㣩 *ע��yΪ������Ƶ׿�ĸ߶� yΪ�ڲ��ݹ�(����)ʹ�õĲ������ⲿ��������ֵ
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
            flag_LoseEdge_part_L=1;
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
            flag_LoseEdge_part_R=1;
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




uint8 Hor_Search(uint8 MidStart,uint8 y)    //ˮƽɨ�ߣ����У� *ע��yΪ������Ƶ׿�ĸ߶�
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
        if(img[y+Y_LIMIT][i]==Black)  //TODO �Ľ��ж����ķ���
        {
            edge[y].Lx=i;                         //ע�⣡����ֱ��ΪĬ��ֵ0 �������Ҳ�Ǹ���0�жϱ����Ƿ�Ϊ���������
            break;
        }

        if(i==X_LIMIT)  //����Ե��δ�ҵ����� �ж϶���
        {
            flag_LoseEdge_part_L=1;
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
            flag_LoseEdge_part_R=1;
        }
    }

    if(y>3) // �������ȶ���
    {
        if(edge[y].Lx-edge[y-2].Lx>=10 || edge[y].Lx-edge[y-2].Lx<=-10) //TODO EdgeLosePos���ܻᴢ����ͬ�ĸ߶ȣ�˫�ඪ��ʱ��
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
        if(edge[y].Lx==0 && edge[y].Rx!=0)
        {
            flag_LoseEdge_part_L=1;
            //edge[y].Lx=edge[y].Rx-Width_Cali(y);
        }

        else if(edge[y].Rx==0 && edge[y].Lx!=0)  //
        {
            flag_LoseEdge_part_R=1;
            //edge[y].Rx=edge[y].Lx+Width_Cali(y);
        }
        else if(edge[y].Rx==0 && edge[y].Lx==0) //�ӵ������ඪ��    
        {                                   //TODO ����˼· ��ֱɨ��Ѱ������ͨ�Ĵ�ֱ·�� ȡ����������λΪ����
            flag_LoseEdge_part_L=1;
            flag_LoseEdge_part_R=1;
            flag_Cross=1;
        }
    }


    return ((edge[y].Rx-edge[y].Lx)/2);
}



//******************************* Base Function *********************************/


//*******************************************************************************/

uint8 Ver_Search(uint8 MidStart)    //��ʼ��ֱɨ��
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

    //EdgeNum-=Y_LIMIT;

    return(EdgeNum);
}





uint8 Uni_Ver_Search(uint8 MidStart)    //��ֱɨ�� ͨ�ô�ֱɨ�� ����ı�EdgeNum��ֵ   *ע�⣡ ��������Ǵ���ױ߿�ʼɨ���
{                                       //������Ϊɶ��д��һ�������������
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




uint8 Feature_Verify(uint8 T_x,uint8 T_y,uint8 dx,uint8 dy,uint8 feature[][])    //�����ȽϺ����������������ͼ���Ӧλ�ý��бȽϣ��������ƶ�(0~100)
{
    int16 rate=0;

    for(uint8 i=0;i<dx;i++)
    {
        for(uint8 j=0;j<dy;j++)
        {
            if(img[T_x+i][T_y+j]&feature[i][j]])    //����������/ͼ��Ƚ�
            {
                rate++;
            }
        }
    }

    rate=(rate/(dx*dy))*100;

    return(rate);
}




uint8 Judge(void)   //TODO ״̬��
{
    if(flag_LoseEdge_part_L*flag_LoseEdge_part_R != 0)  //˫�ඪ���ж�Ϊʮ��
    {
        flag_Cross=1;
    }

    if(flag_LoseEdge_part_L==1 && flag_LoseEdge_part_R!=1)
    {
        flag_Normal_Lose_L=1;   //TODO ��ʱδ������
    }

    if(flag_LoseEdge_part_L!=1 && flag_LoseEdge_part_R==0)
    {
        flag_Normal_Lose_R=1;    //TODO ��ʱδ������
    }

    return 1;
}




inline uint8 Width_Cali(uint8 y)    //���ݸ߶ȼ����������
{
    if(0)   //��ʱ��Ϊ�궨ֵ����
        return((edge[0].Rx-edge[0].Lx)-(WIDTH_K*y)); //ʹ����ײ��ȼ���
    else
        return(WIDTH_BASE-(WIDTH_K*y)); //ʹ�ñ궨ֵ����
}




void Damn_Lose_Edge_all(void)   //˫�ඪ��
{

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
    /*
    uint8 flag_Ver_Raise=0; //��ֱ�߶ȼ���
    uint8 flag_Ver_Drop=0;  //��ֱ�߶ȵ���

    uint8 Ver_Raise_X=0;
    uint8 Ver_Drop_X=0;
    uint8 ver[camera_raw_data_X-X_LIMIT+1]={0}; //��ֱ�߶�����
    uint8 Lose_Edge_Sum=0;  //������
    uint8 Max_Ver=0;
    uint8 Max_Ver_X=0;  //����ߵĺ�����
    uint8 temp=0;

    for(uint8 i=X_LIMIT;i<=camera_raw_data_X-X_LIMIT;i++)
    {   
        ver[i-X_LIMIT]=Uni_Ver_Search(i);   //��������
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
        if(Ver_Drop_X!=0 && Ver_Raise_X!=0) //���߶��и߶�ͻ��
        {
            mid[i]=(Ver_Drop_X+Ver_Raise_X)/2;   //��ʱ�ø�ֵ������
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
        edge[i].Lx=mid[i]-Width_Cali(i)/2;  //FIXME δ���Ǹ�ֵ���
        edge[i].Rx=mid[i]+Width_Cali(i)/2;
    }

    qDebug()<<"Drop:"<<Ver_Drop_X<<" Raise:"<<Ver_Raise_X<<"\n";
    

   for(int i=0;i<=EdgeNum;i++)
   {
       for(int j=0;j<EdgeLoseNum;j++)  //���ñȽ��鷳 ��ֱ��Ƕ����
       {
           if(i==EdgeLosePos[j])    //����
           {
               if(edge[i-1].Lx!=0)  //��������¾��ǰ��� д��ϡ��
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




void If_Lose_Edge(void) //���߲�ȫ
{
    if(flag_Cross)  //���ඪ��
    {
        Damn_Lose_Edge_all();   //˫�ඪ�ߺ���
    }
    else if(flag_Normal_Lose_L)  //���󶪱�
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Lx=edge[i].Rx-Width_Cali(i);    //ɾ�߲�ȫ  ��ֱ�Ӹ�д��߽�����
        }
    }
    else if(flag_Normal_Lose_R)  //���Ҷ���
    {
        for(uint8 i=0;i<=EdgeNum;i++)
        {
            edge[i].Rx=edge[i].Lx+Width_Cali(i);
        }
    }
}




void If_Straight(void)
{
    uint8 SearchTimesMax=EdgeNum-CUR_RESOLUTION;    //�����������

    flag_Straight_L=1;  //�ȼ���Ϊֱ��
    flag_Straight_R=1;

    for(uint8 i=0;i<SearchTimesMax;i++)    //TODO ���ַ����޷����㵽����º����CUR_RESOLUTION/2�����صĽǶ�
    {
        angle[i].L=Get_Angle(i,i+CUR_RESOLUTION,i+CUR_RESOLUTION/2,1);
        angle[i].R=Get_Angle(i,i+CUR_RESOLUTION,i+CUR_RESOLUTION/2,0);
    }

    for(uint8 i=0;i<SearchTimesMax;i++)
    {
        if(angle[i].L<=160)    //��160��Ϊ�� 180�Ǳ�׼ֱ��
        {
            flag_Straight_L=0;
        }

        if(angle[i].R<=160)
        {
            flag_Straight_R=0;
        }
    }
}




void Set_Mid(void)  //�����е�&����Ϊ����(�����������)
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




void Simple_Err_Check(void) //�򵥼�����
{

}





void Print_Mid(void) //�滭����
{
    for(uint8 i=Y_LIMIT; i<=EdgeNum+Y_LIMIT; i++)
    {
        img[i][edge[i-Y_LIMIT].Lx]=Gray;
        img[i][edge[i-Y_LIMIT].Rx]=Gray;

        if(mid[i]<IMG_X && mid[i]>0)    //��Ҳ��֪��Ϊʲô��ʱҪд�����ֵ�����
            img[i][mid[i-Y_LIMIT]]=Gray;
    }

}




void Search(void)   //�ӵ���ɨ��������
{

    uint8 MidStart=IMG_X/2;   //�ױ�������ʼ�������    
    //�������Ҿ����Ҳ�Ӧ�ð�search���hor_search���һ��ͬ���ľֲ��������������ø���
    flag_LoseEdge_all_L=0;
    flag_LoseEdge_all_R=0;

    init();
    Y_Change();

    //-------------------ɨ�߲���---------------------//
    Hor_Search_Base(MidStart);   //��һ����ȷ��ɨ���׼����
    MidStart=(edge[0].Lx+edge[0].Rx)/2;

    Ver_Search(MidStart);   //��ֱɨ�� ����ɨ�����߶� EdgeNum

    for(uint8 i=0;i<=EdgeNum;i++)   //ˮƽɨ��ѭ��
    {
        Hor_Search(MidStart,i);
                                //TODO ȱһ���Զ��߸������ж����� flag_LoseEdge_all_X��λ
    }

    If_Straight();  //ȷ��ֱ��

    if(flag_Straight_L) qDebug()<<"L_Straight\n";
    if(flag_Straight_R) qDebug()<<"R_Straight\n";
    
    //-------------------ɨ�߲���---------------------//


    //-------------------�жϲ���---------------------//
    Judge();
    //-------------------�жϲ���---------------------//


    //-------------------���߲���---------------------//
    If_Lose_Edge(); 

    Set_Mid();
    //-------------------���߲���---------------------//


    //-------------------�滭����---------------------//
    Print_Mid();
    //-------------------�滭����---------------------//



}




void CAM_Error(void)
{
    uint8 unit=EdgeNum/5;

    if(flag_Indct_Help_Me)  //����Ļ�ƫ��ֵ��Ϊ0�ͺ���
    {
        CamError=0;
    }
    else
    {
        CamError=(IMG_X/2)-(mid[unit]+mid[unit*2]+mid[unit*3]+mid[unit*4]+mid[unit*5-3])/5; //��ϵı��ؿ��ŶȽϵ�
    }
    
}

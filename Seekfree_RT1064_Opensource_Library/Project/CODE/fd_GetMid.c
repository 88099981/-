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
uint8 flag_T_Road=0;   //T��·����ָ��ϼ���ȫ�ڣ����³��ִ�������ඪ�ߵ�״��
uint8 flag_Cross=0;    //ʮ��                 ����֡��ʼ����
uint8 flag_Round_ARM_L=0;  //�󻷵�Ԥλ
uint8 flag_Round_ARM_R=0;  //�һ���Ԥλ
uint8 flag_Round_ARM_L_B=0;  //�󻷵�Ԥλ ��������flag_Round_ARM_L/R��ͬһλ�ö����λ���¶�λ�õĴ����ж�  �����á�
uint8 flag_Round_ARM_R_B=0;  //�һ���Ԥλ                                                                �����á�
uint8 flag_Is_This_Round=0;   //�������߱�־  ѯ�ʵ��(������ǵø�λ)    ��Ϊʲô���������溣���أ�
uint8 flag_Round_in_L=0;   //�󻷵����� //ATTENTION ���뻷��ʼ��λ
uint8 flag_Round_in_R=0;   //�һ�������
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

    return 0;
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
        else if(edge[y].Rx==0 && edge[y].Lx==0) //�ӵ������ඪ��    
        {
            flag_LoseEdge_part_L++;
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

    //EdgeNum-=Y_LIMIT;

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
                img[T_y+i][T_x+j]=Gray;

                #ifdef IMG_DEBUG    //TODO Ҫ���ڲ˵������м���ͼ����Զ�Ӧ�ı�־λ�Դ���궨��
                
                #endif
            }
            //img[T_y+i][T_x+j]=Gray;
        }
    }

    rate=(rate/(dx*dy))*100;

    return((uint8)rate);
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

        if(Feature_Verify(0,0,20,10,Block_A)>=90)
        {
            flag_Garage_L=1;
            flag_Garage_R=0;    //��Ϊû��ÿ֡��ʼ��������Ϊ�˱����������ì�ܣ��ʶ�������־λ���ø�
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



//״̬��
uint8 Judge(void)
{
    //-------״̬���� <head>--------//
    
    if(flag_Round_ARM_L)
    {
        flag_Round_ARM_L--;
    }
    else if(flag_Round_ARM_R)
    {
        flag_Round_ARM_R--;
    }
   
    //-------״̬���� <bottom>--------//

    //------������ <head>---------//
    if(If_Garage())
    {
        bb_time=20;
        return 1;
    }
    //------������ <bottom>---------//

     //-------˫�ඪ�� <head>--------//
    if(flag_LoseEdge_part_L*flag_LoseEdge_part_R != 0)  //˫�ඪ���ж�Ϊʮ��
    {
        uint16 NumInBlack=0;

        for(uint8 i=IMG_Y-1;i>IMG_Y-3;i--)  //ɨ����
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

        if(NumInBlack>=IMG_X*1.9) //��������л���ȫ�� ���ֵ���������޸�
        {
            flag_T_Road=1;
            return 1;
        }

        flag_Cross=1;   //T��·�ж����ȼ�����ʮ��

        return 1;
    }
    //-------˫�ඪ�� <bottom>--------//

    //------������� <head>---------//
    do
    {
        if(Feature_Verify(83,20,20,20,Block_A)<=90) //�ж��Ƿ�Ϊֱ��������Сֱ�������
        {
            break;
        }

        if(Feature_Verify(0,19,50,10,Block_A)>=90)  //��Ϊ�еĴ󻷵��뻷�Ƚ���ͣ�û�ж��ߣ������ڶ������ж�
        {
            //ATTENTION �޷����������뻷�ж�(��Ҫ��и���)
            flag_Round_ARM_L=ROUND_ARM_COUNT_TIMES; //���ñ�־λ��Ϊ��ʱ����Ӧ���õ����֤

            //return 1;
        }
        else if(Feature_Verify(137,19,50,10,Block_A)>=90)
        {
            flag_Round_ARM_R=ROUND_ARM_COUNT_TIMES; //���ñ�־λ��Ϊ��ʱ����Ӧ���õ����֤

            //return 1;
        }
    } while (0);    //��дgoto�ֲ���д��м
    
    if(zuo_yuanhuan_flag && flag_Round_ARM_L)    //�뻷�׶�����    ruhuan>=hd_in Ϊ�ⲿ����
    {
        uint8 addition=80;

        for(uint8 i=EdgeNum;i>0;i--)
        {
            addition*=0.96;
            edge[i].Rx-=addition;
        }
        /*
        uint8 Target_y=0;
        uint8 Target_x[2]={0};  //��һԪ��Ϊ��ǰֵ���ڶ�Ԫ��Ϊ�ϴ�ֵ

        for(uint8 i=EdgeNum;i>=IMG_Y/4;i--)   //���󻷣����ϵ��£����ҵ���ɨ�ߣ�Ѱ�Ҽ��
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

            if(i<EdgeNum-1 && Target_x[1]-Target_x[0]>=15) //�仯�ϴ�
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
    else if(you_yuanhuan_flag && flag_Round_ARM_R)   //ruhuan>=hd_in Ϊ�ⲿ��������ʱ�Ѿ����˵�һ������
    {
        uint8 addition=80;

        for(uint8 i=EdgeNum;i>0;i--)
        {
            addition*=0.96;
            edge[i].Lx+=addition;
        }
        /*
        uint8 Target_y=0;
        uint8 Target_x[2]={0};  //��һԪ��Ϊ��ǰֵ���ڶ�Ԫ��Ϊ�ϴ�ֵ

        for(uint8 i=EdgeNum;i>=IMG_Y/4;i--)   //���һ������ϵ��£�������ɨ�ߣ�Ѱ�Ҽ��
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

            if(i<EdgeNum-1 && Target_x[0]-Target_x[1]>=15) //�仯�ϴ�
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

    //------������� <bottom>---------//

    //------AprilTag��� <head>---------//
    if(Feature_Verify_Box(64,3,60,17,2,1)>=75 && Feature_Verify(66,5,56,13,Block_B)>=20)
    {
        bb_time=20;
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



//���߲�ȫ
void If_Lose_Edge(void)
{
    if(flag_Garage_L)
    {
        /*
        edge[0].Rx=IMG_X-1; //�����ұ��ص�����
        edge[IMG_Y-1].Rx=0;

        Connect(edge,0,0,IMG_X-1);  //��������࣬���Ҳ��Ե��Ϊ���ϵ����µ�б��
        EdgeNum=IMG_Y/2;    //�е���Ϊ���߶�һ��
        flag_Normal_Lose_L=1;   //���䲹�Ҳ���
        */
    }
    else if(flag_Garage_R)
    {
        /*
        edge[0].Lx=0; //�����ұ��ص�����
        edge[IMG_Y-1].Lx=IMG_X-1;

        Connect(edge,1,0,IMG_X-1);
        EdgeNum=IMG_Y/2;    //�е���Ϊ���߶�һ��
        flag_Normal_Lose_R=1;   //���䲹�Ҳ���
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

    if(flag_Cross)  //ʮ��
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

    uint8 MidStart=IMG_X/2;   //�ױ�������ʼ�������    
    //�������Ҿ����Ҳ�Ӧ�ð�search���hor_search���һ��ͬ���ľֲ��������������ø���

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
    if(flag_Garage_L || flag_Garage_R)
    {
        break_flag=1;
    }
    //-------------------�������� <bottom>-------------//
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

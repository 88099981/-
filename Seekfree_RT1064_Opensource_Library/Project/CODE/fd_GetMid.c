#include "fd_GetMid.h"

//data
uint8 img[IMG_Y][IMG_X];
EDGE edge[EDGE_MAX];    //�߽�ṹ������ �߽���ֵΪ0��ʱ��Ϊ����
uint8 EdgeNum;    //������
uint8 EdgeLosePos[EDGE_MAX];  //����λ�� (Y��           ����֡��ʼ����
uint8 EdgeLoseNum;   //������                           ����֡��ʼ����
int16 mid[EDGE_MAX];    //TODO ȷ���������������С      ����֡��ʼ����
int8 CamError;

//flag
uint8 flag_LoseEdge_part_L; //�󲿷ֶ���     ����֡��ʼ����
uint8 flag_LoseEdge_part_R; //              ����֡��ʼ����
uint8 flag_LoseEdge_all_L;  //              ����֡��ʼ����
uint8 flag_LoseEdge_all_R;  //              ����֡��ʼ����
uint8 flag_Cross;    //ʮ��                 ����֡��ʼ����
uint8 flag_Round_ARM_L;  //�󻷵�Ԥλ
uint8 flag_Round_ARM_R;  //�һ���Ԥλ
uint8 flag_Is_This_Round;   //�������߱�־  ѯ�ʵ��(������ǵø�λ)    ��Ϊʲô���������溣���أ�
uint8 flag_Round_in_L;   //�󻷵����� ���뻷��ʼ��λ
uint8 flag_Round_in_R;   //�һ�������
uint8 flag_MidStart_in_Black;   //Hor_Search �����и�����ʼ���ں�����    ����֡��ʼ����
uint8 flag_Ver_Search_Finish;   //��ֱɨ�����                          ����֡��ʼ����
uint8 flag_Indct_Help_Me;   //�������ѭ��  ������ǵø�λ               ����֡��ʼ����

//TODO һ�ඪ�ߴ��²���ͨ��һ�߰���������һ�ߵķ�ʽ ����ֱ��ʹ�õ��е�Ͷ��е������ ����������������


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
    //************************

    for(uint8 i=0;i<EDGE_MAX;i++)
    {
        EdgeLosePos[i]=0;
        edge[i].Lx=0;
        edge[i].Rx=0;
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
{                                                  //c99 ֧��Ĭ��ֵ
    flag_MidStart_in_Black=0;   //��־λ����

    if(MidStart>IMG_X-X_LIMIT || MidStart<X_LIMIT || y>IMG_Y-2*Y_LIMIT )  //��Χ��֤
    {
        return 0;
    }

    if(img[y+Y_LIMIT][MidStart]==Black) //����������е��ں�����
    {
        uint8 Sum_of_white_zone=0;  //��ǰ�а������� Ĭ��������� �޸ĸ�����ע��ͬʱ�޸�temp_edge�����Ԫ�ظ���,�Լ�������һ�Ѷ�Ҫ��
        EDGE temp_edge[2];  //��ʱ�߽��

        for(uint8 i=MidStart;i>=X_LIMIT;i--)    //��������
        {
            if(img[y+Y_LIMIT][i]==White)
            {
                temp_edge[0].Rx=i;
                Sum_of_white_zone=1;    //��־�Ѿ������ҵ�����
            }

            if(Sum_of_white_zone==1 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge[1].Lx=i;
            }
        }

        for(uint8 i=MidStart;i<IMG_X-X_LIMIT;i++)    //��������
        {
            if(img[y+Y_LIMIT][i]==White)
            {
                temp_edge[0].Lx=i;
                Sum_of_white_zone=2;    //��־�Ѿ������ҵ�����
            }

            if(Sum_of_white_zone==2 && img[y+Y_LIMIT][i]==Black)
            {
                temp_edge[1].Rx=i;
            }
        }

        if(!Sum_of_white_zone)  //û����
        {
            flag_Indct_Help_Me=1;   //�����л������ѭ��
            return 0;
        }

        if((temp_edge[0].Rx-temp_edge[0].Lx) > (temp_edge[1].Rx-temp_edge[1].Lx))
        {
            MidStart=(temp_edge[0].Rx-temp_edge[0].Lx)/2;   //ȡ�����Ϊ��ǰ���� ��Ȼ�Ҿ���ȡ���������Ҳ��...
        }
        else
        {
            MidStart=(temp_edge[1].Rx-temp_edge[1].Lx)/2;
        }
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
        flag_MidStart_in_Black=1;   //FIXME δ���� �ֺ�������һ����...
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

//****************���߲�ȫ��־λ************************
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

//***********************************************

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

    EdgeNum-=Y_LIMIT;

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




uint8 Judge(void)   //��ʱ����
{
    uint8 Wider=0;  //��������

    for(uint8 i=0;i<EdgeNum;i++)
    {
        if(((edge[i].Rx-edge[i].Lx)-(edge[i+1].Rx-edge[i+1].Lx)) < -3)   //��һ�б��·�һ�п�����Ϊ���������� -3Ϊ�ݲ�
            {
                Wider++;
            }
    }

    if(Wider>3 && Wider<EdgeNum/2) //����������  ��������ʮ�֡������롢�����롢�µ��� ���� <EdgeNum/2 �������Ǽ������µ��ĸ���
    {

    }
    

    return 1;
}




inline uint8 Width_Cali(uint8 y)    //���ݸ߶ȼ���������� �е���͸�ӵ���˼
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

for(uint8 i=EdgeNum;i>=0;i--)
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
    if(flag_LoseEdge_part_L || flag_LoseEdge_part_R)    //ֻҪ�в��ֶ��߾ͻ���ȫ���� �ʲ�����ȫ���ߵ�����
    {
        if(flag_LoseEdge_part_L*flag_LoseEdge_part_R != 0)  //���ඪ��
        {
            //FIXME �޷�����

            //Damn_Lose_Edge_all();   //˫�ඪ�ߺ���    //FIXME ��ʱ�޸�
            Del_Edge_Data(0,EdgeNum-1);
        }
        else if(flag_LoseEdge_part_L==1 && flag_LoseEdge_part_R==0)  //���󶪱�
        {
            for(uint8 i=0;i<=EdgeNum;i++)
            {
                edge[i].Lx=edge[i].Rx-Width_Cali(i);    //ɾ�߲�ȫ  ��ֱ�Ӹ�д��߽�����
            }
            bb_time=20; //����
        }
        else if(flag_LoseEdge_part_L==0 && flag_LoseEdge_part_R==1)  //���Ҷ���
        {
            for(uint8 i=0;i<=EdgeNum;i++)
            {
                edge[i].Rx=edge[i].Lx+Width_Cali(i);
            }
            bb_time=20; //����
        }
    }

    flag_LoseEdge_part_L=0; //���ñ�־λ
    flag_LoseEdge_part_R=0;
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
            //mid[i]=edge[i].Rx-Width_Cali(i)/2;    
            //mid[i]=edge[i-1].Rx-mid[i-1];
        }

        else if(edge[i].Rx==0 && edge[i].Lx!=0)  //
        {
            mid[i]=(edge[i].Lx+edge[i].Rx)/2;
            //mid[i]=edge[i].Lx-Width_Cali(i)/2;
            //mid[i]=mid[i-1]-edge[i].Lx;
        }

        else
        {
            mid[i]=94;  //ֱ�������е�
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

    Hor_Search_Base(MidStart,0);   //��һ����ȷ��ɨ���׼����
    MidStart=(edge[0].Lx+edge[0].Rx)/2;    //TODO ��ȷ���ײ��Ե��ֵ ��Ӹ�������

    Ver_Search(MidStart);   //��ֱɨ�� ����ɨ�����߶� EdgeNum

    for(uint8 i=0;i<=EdgeNum;i++)   //ˮƽɨ��ѭ��
    {
        Hor_Search(MidStart,i);
                                //TODO ȱһ���Զ��߸������ж����� flag_LoseEdge_all_X��λ
    }

    If_Lose_Edge(); 

    Set_Mid();
    Print_Mid();
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

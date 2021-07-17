#include "ostu.h"
#include "cut.h"

#define Step_yu    252				//4步   等差  灰度级
#define Step   4

uint8 ostu_thres=0;

uint8 get_ostu_thres()
{
	uint16 i, j;
	
	//所使用的图像的范围 
	uint8 height_start=10;
	uint8 height = IMG_H;
	uint8 width_start=0;
	uint8 width = IMG_W;
	
	uint16 histogram[256]={0};						//直方图 
	uint16 histogram_cdf[256]={0};					//直方图的累计分布图最大IMG_H*IMG_W 
	uint32 histogram_u_cdf[256]={0};				//最大IMG_H*IMG_W*255
	
    uint8 find_gray_start=0;						//找到灰度值起始点的标志位 
	uint8 gray_start;								//图像的起始灰度
	uint8 gray_end;									//图像的终止灰度 
	uint8 gray_ave;									//图像的平均灰度值
	 
	uint8 bisection_left,bisection_right;			//二分法的起始左和右 
    uint8 bisection_max_dis=Step*2;					//二分法左右两端的最大差距
    uint16 n0,n1;									//阈值左右两边的像素点数 
    uint64 class_var_left,class_var_right;			//左右两边的类间方差 
	uint64 class_var_i;								//i处的类间方差
	 
	//做出灰度直方图
	for(i=height_start;i<height;i++)
	{
		for(j = width_start;j<width;j++)
		{
			histogram[img[i][j]&Step_yu]++;			//Step_yu等于252时，相当于与上0xFC,忽略末尾四位 
		}
	}
	
	histogram_cdf[0]=histogram[0];
	histogram_u_cdf[0]=0;
    for(i=Step;i<256;i=i+Step)
    {
    	histogram_cdf[i]=histogram_cdf[i-Step]+histogram[i];
		histogram_u_cdf[i]=histogram_u_cdf[i-Step]+histogram[i]*i;
		if(!find_gray_start&&histogram[i])
		{
			gray_start=i;
		}
		else if(find_gray_start&&!histogram[i])
		{
			gray_end=i-Step;
		}
	}
	gray_ave=(uint8)(histogram_u_cdf[Step_yu]/(uint32)histogram_cdf[Step_yu])&Step_yu;
	
	bisection_left=(uint8)(histogram_u_cdf[gray_ave]/(uint32)histogram_cdf[gray_ave])&Step_yu;
	bisection_right=(uint8)((histogram_u_cdf[Step_yu]-histogram_u_cdf[gray_ave])/
					(uint32)(histogram_cdf[Step_yu]-histogram_cdf[gray_ave]))&Step_yu;
	
	n0=histogram_cdf[bisection_left];
	n1=histogram_cdf[Step_yu]-n0;			//histogram_cdf[255]就是取用的图像的像素点的多少 
	class_var_left=((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[bisection_left]*histogram_cdf[Step_yu])*
					((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[bisection_left]*histogram_cdf[Step_yu])/(uint64)n0
					+(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[bisection_left])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)*
					(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[bisection_left])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)/(uint64)n1;
	
	n0=histogram_cdf[bisection_right];
	n1=histogram_cdf[Step_yu]-n0;			//histogram_cdf[255]就是取用的图像的像素点的多少 
	class_var_right=((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[bisection_right]*histogram_cdf[Step_yu])*
					((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[bisection_right]*histogram_cdf[Step_yu])/(uint64)n0
					+(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[bisection_right])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)*
					(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[bisection_right])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)/(uint64)n1;
	
	//二分法 
	for(i=((bisection_left+bisection_right)>>1)&Step_yu;(bisection_right-bisection_left)>=bisection_max_dis;i=((bisection_left+bisection_right)>>1)&Step_yu)
	{
		n0=histogram_cdf[i];
		n1=histogram_cdf[Step_yu]-n0;			//histogram_cdf[255]就是取用的图像的像素点的多少
		
		class_var_i=((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[i]*histogram_cdf[Step_yu])*
					((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[i]*histogram_cdf[Step_yu])/(uint64)n0
					+(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[i])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)*
					(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[i])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)/(uint64)n1;
		
		if(class_var_i>=class_var_left&&class_var_right>=class_var_left)		//替换掉左边的值 
		{
			bisection_left=i;
			class_var_left=class_var_i;
		}
        else if(class_var_i>=class_var_right&&class_var_left>=class_var_right)	//替换掉右边的值 
        {
			bisection_right=i;
			class_var_right=class_var_i;
        }
        ////排除干扰的情况下出现的中间的值最小的时候  就是死循环了
        else if(class_var_i<=class_var_left&&class_var_i<=class_var_right)
        {
          	//左右两边取最大的
            if(class_var_left>=class_var_right)
			{
				bisection_right=i;               //替换右边
				class_var_right=class_var_i;
			}
            else
			{
				bisection_left=i;                //替换左边
				class_var_left=class_var_i;
			}
        }
	}
	
	n0=histogram_cdf[i];
	n1=histogram_cdf[Step_yu]-n0;			//histogram_cdf[255]就是取用的图像的像素点的多少
	
	class_var_i=((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[i]*histogram_cdf[Step_yu])*
				((uint64)histogram_u_cdf[Step_yu]*n0-(uint64)histogram_u_cdf[i]*histogram_cdf[Step_yu])/(uint64)n0
				+(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[i])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)*
				(((uint64)histogram_u_cdf[Step_yu]-(uint64)histogram_u_cdf[i])*(uint64)histogram_cdf[Step_yu]-(uint64)histogram_u_cdf[Step_yu]*n1)/(uint64)n1;
	if(class_var_i<class_var_left)
	{
		i=bisection_left;
	}
	else if(class_var_i<class_var_right)
	{
		i=bisection_right;
	}
        
	return i;
}

void binary_img()
{
    //uint8 in_ostu_thres=85;
	uint8 in_ostu_thres=get_ostu_thres();
	uint8 i,j;
	for(i=0;i<IMG_H;i++)
	{
		for(j=0;j<IMG_W;j++)
		{
			if(img[i][j]>in_ostu_thres)
				img[i][j]=White;
			else
				img[i][j]=Black;
		}
	}
}

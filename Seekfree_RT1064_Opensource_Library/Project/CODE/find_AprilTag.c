#include "headfile.h"

#define IMG_X 188
#define IMG_Y 50

typedef const char uint8;

uint8 pix_img[50][188];
// 以下为添加部分

#define PIX_IMG_X IMG_X/3
#define PIX_IMG_Y IMG_Y/3


void Find_AprilTag(const uint8 T_y)
{
    uint8 A_x=0;
    uint8 A_y=T_y;
    uint8 WofR=0;
    uint8 WofUP=0;
    uint8 WofDOWN=0;

    for(uint8 i=0;i<PIX_IMG_X/3-1;i++)
    {
        if(pix_img[A_y][i]==White && pix_img[A_y][i+1]==Black)
        {
            A_x=i+2;
            break;
        }
    }

    for(uint8 i=A_x;i<A_x+8;i++)    //从基准点向右扫8个
    {
        if(i>=PIX_IMG_X)
        {
            break;
        }

        if(pix_img[A_y][i]==White)
        {
            WofR++;
        }
    }

    for(uint8 j=A_y;j<A_y+8;j++)
    {
        if(pix_img[j][A_x]==White)
        {
            WofUP++;
        }
    }

    for(uint8 j=A_y;j>=0;j--)
    {
        if(pix_img[j][A_x]==White)
        {
            WofDOWN++;
        }
    }

    if(WofR>=2 && WofUP>=3 && WofDOWN>=1)
    {
        flag_AprilTag=1;
    }
}
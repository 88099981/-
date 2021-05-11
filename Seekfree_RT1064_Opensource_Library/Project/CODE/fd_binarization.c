#include "fd_binarization.h"

uint16 threshold=0; //阈值

void SimpleThreshold(void) //简单二值化（五点采样）	FIXME:未考虑冲出后的情况
{
	int imgw = IMG_W / 4;
	int imgh = IMG_H / 4;
	int avrval[5];
	avrval[0] = img[imgh][imgw];
	avrval[1] = img[imgh*3][imgw];
	avrval[2] = img[imgh*3][imgw*2];
	avrval[3] = img[imgh][imgw*3];
	avrval[4] = img[imgh*3][imgw*3];
	threshold = (avrval[0] + avrval[1] + avrval[2] + avrval[3] + avrval[4])/5;
	threshold = threshold;
}

void SimpleBinarization(void) //单阈值全局二值化
{
	for (int i = 0; i < IMG_H; i++)
	{
		for (int j = 0; j < IMG_W; j++)
		{
			if (img[i][j] <= threshold) img[i][j] = Black; //小于阈值的为黑色
			else img[i][j] = White;
		}

	}
}

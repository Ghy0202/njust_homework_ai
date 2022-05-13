///////////////////////////////////////////////////////////////////////////////////////////
//
//   第三章
//
///////////////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <nmmintrin.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//中值滤波
//
///////////////////////////////////////////////////////////////////////////////////////
void GetMedianGry(int *histogram, int N, int *medGry)
{
	int g;
	int num;

	// step.1-------------求灰度中值------------------------//
	num = 0;
	for (g = 0; g<256; g++)
	{
		num += histogram[g];
		if (2*num>N) break;  //num>N/2
	}
	*medGry = g;
	// step.2-------------结束------------------------------//
	return;
}

double RmwMedianFilter( BYTE *pGryImg, int width, int height,
	                    int M, int N, //滤波邻域：M列N行
	                    BYTE *pResImg
                      )
{   //是每行建一个直方图,没有进行相邻行直方图递推和图像边界变窗口等优化处理
	BYTE *pCur, *pRes;
	int halfx,halfy,x, y, i, j, y1, y2;
	int histogram[256];
	int wSize, j1, j2;
	int num, med, v;
	int dbgCmpTimes = 0; //搜索中值所需比较次数的调试

	M = M/2*2+1; //奇数化
	N = N/2*2+1; //奇数化
	halfx = M/2; //x半径
	halfy = N/2; //y半径
	wSize = (halfx*2+1)*(halfy*2+1); //邻域内像素总个数
	for (y = halfy, pRes = pResImg+y*width; y<height-halfy; y++)
	{
		//step.1----初始化直方图
		y1 = y-halfy;
		y2 = y+halfy;
		memset(histogram, 0, sizeof(int)*256);
		for (i = y1, pCur = pGryImg+i*width; i<=y2; i++, pCur += width)
		{
			for (j = 0; j<halfx*2+1; j++)
			{
				histogram[*(pCur+j)]++;
			}
		}
		//step.2-----初始化中值
		num = 0; //记录着灰度值从0到中值的个数
		for (i = 0; i<256; i++)
		{
			num += histogram[i];
			if (num*2>wSize)
			{
				med = i;
				break;
			}
		}
		//滤波
		pRes += halfx; //没有处理图像左边界侧的像素
		for (x = halfx; x<width-halfx; x++)
		{
			//赋值
			*(pRes++) = med;
			//step.3-----直方图递推: 减去当前邻域最左边的一列,添加邻域右侧的一个新列
			j1 = x-halfx; //最左边列
			j2 = x+halfx+1; //右边的新列
			for (i = y1, pCur = pGryImg+i*width; i<=y2; i++, pCur += width)
			{
				//减去最左边列
				v = *(pCur+j1);
				histogram[v]--;  //更新直方图
				if (v<=med) num--; //更新num
				//添加右边的新列
				v = *(pCur+j2);
				histogram[v]++; //更新直方图
				if (v<=med) num++; //更新num
			}
			//step.4-----更新中值
			if (num*2<wSize) //到上次中值med的个数不够了,则med要变大
			{
				for (med = med+1; med<256; med++)
				{
					dbgCmpTimes += 2; //总的比较次数,调试用
					num += histogram[med];
					if (num*2>wSize) break;
				}
				dbgCmpTimes += 1; //总的比较次数,调试用
			}
			else //到上次中值med的个数多了,则med要变小
			{
				while ((num-histogram[med])*2>wSize)//若减去后,仍变小
				{
					dbgCmpTimes++; //总的比较次数,调试用
					num -= histogram[med];
					med--;
				}
				dbgCmpTimes += 2; //总的比较次数,调试用
			}
		}
		pRes += halfx;//没有处理图像右边界侧的像素
	}
	//返回搜索中值需要的平均比较次数
	return dbgCmpTimes*1.0/((width-halfx*2)*(height-halfy*2));
}


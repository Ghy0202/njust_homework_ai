///////////////////////////////////////////////////////////////////////////////////////////
//
//第五章
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

///////////////////////////////////////////////////////////////////////////////////////////
//
//二值化
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwThreshold(BYTE *pGryImg, int width, int height, int thre)
{
	BYTE *pCur, *pEnd = pGryImg+width*height;

	for (pCur = pGryImg; pCur<pEnd; pCur++) *pCur = (*pCur>=thre)*255;
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Otsu阈值
//
///////////////////////////////////////////////////////////////////////////////////////////
int RmwGetOtsuThreshold(int *histogram, int nSize)
{
	int thre;
	int i, gmin, gmax;
	double dist, f, max;
	int s1, s2, n1, n2, n;

	// step.1-----确定搜索范围:最小值----------------------------//
	gmin = 0;
	while (histogram[gmin]==0) ++gmin;
	// step.2-----确定搜索范围:最大值----------------------------//
	gmax = nSize-1;
	while (histogram[gmax]==0) --gmax;
	// step.3-----搜索最佳阈值-----------------------------------//
	if (gmin==gmax) return gmin; //不满足二类分布
	max = 0;
	thre = 0;
	//初始化u1
	s1 = n1 = 0;
	//初始化u2
	for (s2 = n2 = 0, i = gmin; i<=gmax; i++)
	{
		s2 += histogram[i]*i;
		n2 += histogram[i];
	}
	//搜索
	for (i = gmin, n = n2; i<gmax; i++)
	{   //加速
		if (!histogram[i]) continue;
		//更新s1s2
		s1 += histogram[i]*i;
		s2 -= histogram[i]*i;
		//更新n1n2
		n1 += histogram[i];
		n2 -= histogram[i];
		//评价函数
		dist = (s1*1.0/n1-s2*1.0/n2);
		f = dist*dist*(n1*1.0/n)*(n2*1.0/n);
		if (f>max)
		{
			max = f;
			thre = i;
		}
	}
	// step.4-----返回-------------------------------------------//
	return thre+1; //二值化时是用>=thre,所以要+1
}
int my_RmwGetOtsuThreshold(int* histogram, int nSize,int gmin,int gmax)
{
	int thre;
	//int i, gmin, gmax;
	int i;
	double dist, f, max;
	int s1, s2, n1, n2, n;

	if (gmin == gmax) return gmin; //不满足二类分布
	max = 0;
	thre = 0;
	//初始化u1
	s1 = n1 = 0;
	//初始化u2
	for (s2 = n2 = 0, i = gmin; i <= gmax; i++)
	{
		s2 += histogram[i] * i;
		n2 += histogram[i];
	}
	//搜索
	for (i = gmin, n = n2; i < gmax; i++)
	{   //加速
		if (!histogram[i]) continue;
		//更新s1s2
		s1 += histogram[i] * i;
		s2 -= histogram[i] * i;
		//更新n1n2
		n1 += histogram[i];
		n2 -= histogram[i];
		//评价函数
		dist = (s1 * 1.0 / n1 - s2 * 1.0 / n2);
		f = dist * dist * (n1 * 1.0 / n) * (n2 * 1.0 / n);
		if (f > max)
		{
			max = f;
			thre = i;
		}
	}
	// step.4-----返回-------------------------------------------//
	return thre + 1; //二值化时是用>=thre,所以要+1
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//画直方图
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwDrawHistogram2Img( int *histogram, int histSize, 
	                       double maxScale,
	                       BYTE *pGryImg, int width, int height
                         )
{   //直方图数据画为黑色
	int x1, x2, y1;
	int maxV, i;
	int x, y;

	// step.1-------------初始化----------------------------//
	memset(pGryImg, 225, width*height);
	// step.2-------------寻找最大值------------------------//
	maxV = 0;
	for (i = 0; i<histSize; i++)
	{
		maxV = max(maxV, histogram[i]);
	}
	maxV = (int)(maxV*maxScale);
	// step.3-------------像素填充--------------------------//
	for (i = 0; i<histSize; i++)
	{
		y1 = (height-1)-histogram[i]*(height-1)/(maxV+1);
		y1 = max(0, y1);
		x1 = i*width/histSize;
		x2 = (i+1)*width/histSize;
		for (x = x1; x<x2; x++)
		{
			for (y = height-1; y>y1; y--) *(pGryImg+y*width+x) = 0;
		}
	}
	// step.4-------------结束------------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//二维直方图
//
///////////////////////////////////////////////////////////////////////////////////////////
void Rmw2DHistogram(BYTE *pGryImg, BYTE *pAvrImg, int width, int height, int *histogram2D)
{   
	int *pos;

	memset(histogram2D, 0, sizeof(int)*256*256);
	for (int i = 0; i<width*height; i++)
	{
		pos= histogram2D+pGryImg[i]*256+pAvrImg[i];
		*pos = *pos + 1;
	}
	return;
}

int RmwHistogramBy2D( BYTE *pGryImg, BYTE *pAvrImg, 
	                  int width, int height, 
	                  int dist, //与均值的偏差
	                  int *histogram
                    )
{
	int g, u;
	int num = 0;

	memset(histogram, 0, sizeof(int)*256);
	for (int i = 0; i<width*height; i++)
	{
		u = pAvrImg[i];
		g = pGryImg[i];
		if (abs(g-u)<=dist) //仅统计在均值附近者
		{
			histogram[g]++;
			num++;
			//pGryImg[i] = 255; //调试
		}
	}
	return num;
}

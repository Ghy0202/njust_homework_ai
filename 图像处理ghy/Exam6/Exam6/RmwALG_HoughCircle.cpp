///////////////////////////////////////////////////////////////////////////////////////////
//
//第六章-圆霍夫变换
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
#include "RmwDraw1Data2Img.h"
#include "bmpFile.h"

static BYTE gDbgImg[1024*1024]; //一个全局的调试用图像
///////////////////////////////////////////////////////////////////////////////////////////
//
//画圆
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwDrawCircle(BYTE *pGryImg, int width, int height, int x0, int y0, int r, int color)
{
	double theta;
	int x, y;

	for (theta = 0; theta<360; theta+=0.5) //步长为1度
	{
		for (int i = -2; i < 3; i++) {
			x = x0 + (int)((r+i) * cos(theta * 3.1415926 / 180));
			y = y0 + (int)((r+i) * sin(theta * 3.1415926 / 180));
			if ((x >= 0) && (x < width) &&
				(y >= 0) && (y < height)
				)
			{
				*(pGryImg + y * width + x) = color;
			}
		}
		
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//圆HT变换
//
///////////////////////////////////////////////////////////////////////////////////////////
int RmwHoughCircle( int *pCount, //外部申请好的计数空间,大小width*height
	                int width, int height,  //图像的大小
	                int r, //圆的半径 
	                int *x, int *y, int N, //边缘点
	                int *x0,int *y0 //圆心位置(限定圆心位置在图像内部)
                  )
{  	//已知半径r时的Hough变换
	int *pCur;
	int theta, cosV, sinV;
	int i, tstY0, tstX0, maxCount;

 	// step.1-------------霍夫变换---------------------------//
	memset(pCount, 0, sizeof(int)*width*height);
	for (theta = 0; theta<360; theta++) //步长为1度
	{
		cosV = (int)(cos(theta*3.1415926/180)*2048); //放大2048倍
		sinV = (int)(sin(theta*3.1415926/180)*2048);
		for (i = 0; i<N; i++)
		{
			tstX0 = x[i]-((r*cosV)>>11); //缩小2048倍,步长为1
			if ((tstX0<0)||(tstX0>width-1)) continue; //tstX0无效
			tstY0 = y[i]-((r*sinV)>>11); //缩小2048倍,步长为1
			if ((tstY0<0)||(tstY0>height-1)) continue; //tstY0无效
			pCount[tstY0*width+tstX0] += 1; //计数
		}
	}
	// step.2-------------寻找最大值位置----------------------//
	*x0 = *y0 = maxCount = 0;
	for (tstY0 = 0, pCur = pCount; tstY0<height; tstY0++)
	{
		for (tstX0 = 0; tstX0<width; tstX0++, pCur++)
		{
			if (*pCur>maxCount)
			{
				maxCount = *pCur;
				*x0 = tstX0;
				*y0 = tstY0;
			}
		}
	}
	// step.3-------------结束------------------------------//
	return maxCount; //由于计算误差等原因,此值不代表在出现在结果圆上的边缘点数
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//分治法求圆心
//
///////////////////////////////////////////////////////////////////////////////////////////
int RmwCircleX0(BYTE *pBinImg, int width, int height, int *x0)
{
	int *pCount, x, y, x1, x2, maxCount;
	BYTE *pRow;

 	// step.1-------------申请计数器空间---------------------//
	pCount = new int[width]; //建议pCount在该函数外申请
	if (!pCount) return 0;
	// step.2-------------中心位置计数-----------------------//
	memset(pCount, 0, sizeof(int)*width);
	for (y = 0, pRow = pBinImg; y<height; y++, pRow += width)
	{
		//最左侧的黑点
		for (x1 = 0; x1<width; x1++)
		{
			if (!pRow[x1]) break;
		}
		//最右侧的黑点
		for (x2 = width-1; x2>=0; x2--)
		{
			if (!pRow[x2]) break;
		}
		//中点计数
		if (x2-x1>0)
		{
			pCount[(x2+x1+1)>>1]++;
 		}
	}
	// step.3-------------寻找最大值位置----------------------//
	*x0 = 0;
	maxCount = 0;
	for (x = 0; x<width; x++)
	{
		if (pCount[x]>maxCount) { maxCount = pCount[x]; *x0 = x; }
	}
	// step.3-------------结束------------------------------//
	//调试
#if 1
	memcpy(gDbgImg, pBinImg, width*height);
	RmwDrawData2ImgCol(pCount, height, 1.0, gDbgImg, width, height, 254);
	Rmw_Write8BitImg2BmpFileMark(gDbgImg, width, height, "..\\RmwCPP0602\\Fig0617a_x0.bmp");
#endif
	delete pCount; //释放自己申请的内存,该函数内动态申请和释放会产生内存碎片
	return maxCount;
}

int RmwCircleY0(BYTE *pBinImg, int width, int height, int *y0)
{
	int *pCount, x, y, y1, y2, maxCount;
	BYTE *pRow;

 	// step.1-------------申请计数器空间---------------------//
	pCount = new int[height]; //建议pCount在该函数外申请
	if (!pCount) return 0;
	// step.2-------------中心位置计数-----------------------//
	memset(pCount, 0, sizeof(int)*width);
	//上下
	for (x = 0; x<width; x++)
	{
		//最上面的黑点
		for (y1 = 0, pRow = pBinImg+x; y1<height; y1++, pRow += width)
		{
			if (!(*pRow)) break;
		}
		//最下面的黑点
		for (y2 = height-1, pRow = pBinImg+y2*width+x; y2>=0; y2--, pRow -= width)
		{
			if (!(*pRow)) break;
		}
		//中点计数
		if (y2-y1>0)
		{
			pCount[(y2+y1+1)>>1]++;
 		}
	}
	// step.3-------------寻找最大值位置----------------------//
	*y0 = 0;
	maxCount = 0;
	for (y = 0; y<height; y++)
	{
		if (pCount[y]>maxCount) { maxCount = pCount[y]; *y0 = y; }
	}
	// step.3-------------结束------------------------------//
	//调试
#if 1
	memcpy(gDbgImg, pBinImg, width*height);
	RmwDrawData2ImgRow(pCount, height, 1.0, gDbgImg, width, height, 254);
	Rmw_Write8BitImg2BmpFileMark(gDbgImg, width, height, "..\\RmwCPP0602\\Fig0617a_y0.bmp");
#endif
	delete pCount;//释放自己申请的内存,该函数内动态申请和释放会产生内存碎片
	return maxCount;
}

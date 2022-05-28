///////////////////////////////////////////////////////////////////////////////////////////
//
//第六章-直线霍夫变换
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

#include "bmpFile.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
//画直线
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwDrawKBLine(BYTE *pGryImg, int width, int height, bool isYKX, double K, double B, int color)
{
	int x, y;

	if (isYKX) // y=kx+b
	{
		for (x = 0; x<width; x++)
		{
			y = (int)(K*x+B+0.5);
			if ((y>=0)&&(y<height))
			{
				*(pGryImg+y*width+x) = color;
			}
		}
	}
	else //x=ky+b
	{
		for (y = 0; y<height; y++)
		{
			x = (int)(K*y+B+0.5);
			if ((x>=0)&&(x<width))
			{
				*(pGryImg+y*width+x) = color;
			}
		}
	}
	return;
}
void RmwDrawABCLine( BYTE *pGryImg, int width, int height,
	                 double A, double B, double C,
	                 int color
                   )
{
	int x, y;
 
	if (fabs(B)>fabs(A))
	{
		for (x = 0; x<width; x++)
		{
			y = (int)(-(C+A*x)/B+0.5);
			if ((y>=0)&&(y<height))  *(pGryImg+y*width+x) = color;
		}
	}
	else
	{
		for (y = 0; y<height; y++)
		{
			x = (int)(-(C+B*y)/A+0.5);
			if ((x>=0)&&(x<width))  *(pGryImg+y*width+x) = color;
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//直线拟合
//
///////////////////////////////////////////////////////////////////////////////////////////
bool RmwFittingLine(int *x, int *y, int N, double *k, double *b)
{
	bool isYKX;
	double sumx, sumy, sumxy, avrx, avry;
	int i;

	// step.1-------------计算坐标均值----------------------//
	sumx = sumy = 0.0;
	for (i = 0; i<N; i++)
	{
		sumx += x[i];
		sumy += y[i];
	}
	avrx = sumx/N;
	avry = sumy/N;
	// step.2-------------计算坐标方差----------------------//
	sumx = sumy = sumxy = 0;
	for (i = 0; i<N; i++)
	{
		sumx += (x[i]-avrx)*(x[i]-avrx);
		sumy += (y[i]-avry)*(y[i]-avry);
		sumxy += (x[i]-avrx)*(y[i]-avry);
	}
	// step.3-------------判断直线类型和求kb----------------//
	if (sumx>sumy) //y=kx+b
	{
		isYKX = true;
		*k = sumxy/sumx;
		*b = avry-(*k)*avrx;
	}
	else //x=ky+b
	{
		isYKX = false;
		*k = sumxy/sumy;
		*b = avrx-(*k)*avry;
	}
	// step.4-------------返回直线类型----------------------//
	return isYKX;
}
void RmwFittingLine(int *x, int *y, int N, double *A, double *B, double *C)
{
	bool isYKX;
	double k, b;

	// step.1-------------执行斜截式直线拟合-----------------//
	isYKX = RmwFittingLine(x, y, N, &k, &b);
	// step.2-------------将斜截式转换为一般式---------------//
	if (isYKX) //y=kx+b => kx-y+b=0
	{
		*A = k;
		*B = -1;
		*C = b;
	}
	else //x=ky+b => x-ky-b=0
	{
		*A = 1;
		*B = -k;
		*C = -b;
	}
	// step.3-------------返回-----------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//直线HT变换
//
///////////////////////////////////////////////////////////////////////////////////////////
int RmwHoughLine( int width,int height,
	              int *x, int *y, int N,
	              double *A, double *B, double *C,int*pCount
                )
{
	int maxCount, bstTheta, bstThro;
	int*pCenter,*pCur;
	int maxThro, cosV, sinV;
	int theta, thro, i;

	// step.1-------------申请计数器空间---------------------//
	maxThro = (int)sqrt(1.0*width*width+height*height+0.5)+1;
	//pCount = new int[(maxThro*2)*180]; //(-maxThro,maxThro)
	if (!pCount) return 0; //建议pCount在该函数外申请
	// step.2-------------霍夫变换---------------------------//
	memset(pCount, 0, sizeof(int)*(maxThro*2)*180);
	for (theta = 0; theta<180; theta++) //步长为1度
	{
		cosV = (int)(cos(theta*3.1415926/180)*8192); //放大8192倍
		sinV = (int)(sin(theta*3.1415926/180)*8192);
		pCenter = pCount+(maxThro*2)*theta+maxThro; //加上偏移maxThro
		for (i = 0; i<N; i++)
		{
			thro = ((x[i]*cosV+y[i]*sinV)>>13); //缩小8192倍,thro的步长为1
 			*(pCenter+thro) += 1;
		}
	}
	// step.3-------------最大值搜索-------------------------//
	maxCount = bstTheta = bstThro = 0;
	for (theta = 0,pCur = pCount; theta<180; theta++)
	{
		for (thro = 0; thro<maxThro*2; thro++, pCur++)
		{
			if (*pCur>maxCount)
			{
				maxCount = *pCur;
				bstTheta = theta;
				bstThro = thro;
			}
		}
	}
	bstThro -= maxThro; //去掉偏移maxThro
	// step.4-------------求直线Ax+By+C的值-----------------//
	//x*cos(bstTheta)+y*sin(bstTheta)=bstThro => Ax+By+C=0
	*A = cos(bstTheta*3.1415926/180);
	*B = sin(bstTheta*3.1415926/180);
	*C = -bstThro;
	printf("%lfX+%lfY+%lf\n", *A, *B, *C);
	printf("theta=%d,thro=%d\n", bstTheta, bstThro);
	unsigned char* pCountImg = new unsigned char[maxThro * 2 * 180];
	for (int k = 0; k < maxThro * 2 * 180; k++) {
		if (pCount[k] != 0) {
			pCountImg[k] = 255;
		}
		else {
			pCountImg[k] = 0;
		}

	}
	char tname[90];
	Rmw_Write8BitImg2BmpFile(pCountImg, maxThro, 2 * 180, "bbb.bmp");
	// step.5-------------返回经过的点数---------------------//
 	//delete pCount; //释放自己申请的内存,该函数内动态申请和释放会产生内存碎片
	return maxCount;
}

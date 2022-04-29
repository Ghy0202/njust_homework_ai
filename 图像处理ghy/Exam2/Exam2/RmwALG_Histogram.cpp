///////////////////////////////////////////////////////////////////////////////////////////
//
//   第二章
//
///////////////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//直方图及其计算
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwGetHistogram(BYTE *pGryImg, int width, int height, int *histogram)
{
	BYTE *pCur, *pEnd = pGryImg+width*height;

	// step.1-------------初始化----------------------------//
	//for (int g = 0; g<256; g++) histogram[g] = 0;
	memset(histogram, 0, sizeof(int)*256);
	// step.2-------------直方图统计------------------------//
	for (pCur = pGryImg; pCur<pEnd; ) histogram[*(pCur++)]++;
	// step.3-------------结束------------------------------//
	return;
}

void GetMinMaxGry(int *histogram, int *minGry, int *maxGry)
{
	int g;

	// step.1-------------求最小值--------------------------//
	for (g = 0; g<256; g++)
	{
		if (histogram[g]) break;
	}
	*minGry = g;
	// step.2-------------求最大值--------------------------//
	for (g = 255; g>=0; g--)
	{
		if (histogram[g]) break;
	}
	*maxGry = g;
	// step.3-------------结束------------------------------//
	return;
}

void GetBrightContrast(int *histogram, double *bright, double *contrast)
{
	int g;
	int sum, num;
	double fsum;

	// step.1-------------求亮度----------------------------//
	for (sum = num = 0, g = 0; g<256; g++)
	{
		sum += histogram[g]*g;
		num += histogram[g];
	}
	*bright = sum*1.0/num;
	// step.2-------------求对比度--------------------------//
	for (fsum = 0.0, g = 0; g<256; g++)
	{
		fsum += histogram[g]*(g-*bright)*(g-*bright);
	}
	*contrast = sqrt(fsum/(num-1));
	// step.3-------------结束------------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwHistogramEqualizeDemo(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur, *pEnd = pGryImg+width*height;
	int histogram[256], A[256], LUT[256], g;

	// step.1-------------求直方图--------------------------//
	memset(histogram, 0, sizeof(int)*256);
	for (pCur = pGryImg; pCur<pEnd;) histogram[*(pCur++)]++;
	// step.2-------------求A[g],N-------------------------//
	for (g = 1, A[0] = histogram[0]; g<256; g++)
	{
		A[g]= A[g-1]+histogram[g];
	}
	// step.3-------------求LUT[g]-------------------------//
	for (g = 0; g<256; g++) LUT[g] = 255*A[g]/(width*height);
	// step.4-------------查表------------------------------//
	for (pCur = pGryImg; pCur<pEnd;) *(pCur++) = LUT[*pCur];
	// step.5-------------结束------------------------------//
	return;
}

void RmwHistogramEqualize(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur, *pEnd = pGryImg+width*height;
	int histogram[256], LUT[256], A, g;

	// step.1-------------求直方图--------------------------//
	memset(histogram, 0, sizeof(int)*256);
	for (pCur = pGryImg; pCur<pEnd;) histogram[*(pCur++)]++;
	// step.2-------------求LUT[g]-------------------------//
	A = histogram[0];
	LUT[0] = 255*A/(width*height);
	for (g = 1; g<256; g++)
	{
		A += histogram[g];
		LUT[g] = 255*A/(width*height);
		//LUT[g] = 15*A/(width*height)*16;
		//LUT[g] = A/(width*height)*255;
		//LUT[g] = LUT[g-1]+255*histogram[g]/(width*height);
		//histogram[g] += histogram[g-1];
		//LUT[g] = 255*histogram[g]/(width*height);
	}
	// step.3-------------查表------------------------------//
	for (pCur = pGryImg; pCur<pEnd;) *(pCur++) = LUT[*pCur];
	// step.4-------------结束------------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//对数变换
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwLogTransform(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur, *pEnd = pGryImg+width*height;
	int histogram[256], LUT[256], gmax, g;
	double c;

	// step.1-------------求直方图--------------------------//
	memset(histogram, 0, sizeof(int)*256);
	for (pCur = pGryImg; pCur<pEnd;) histogram[*(pCur++)]++;
	// step.2-------------最大值---------------------------//
	for (gmax = 255; gmax>=0; gmax++) if (histogram[gmax]) break;
	// step.3-------------求LUT[g]-------------------------//
	c = 255.0/log(1+gmax);
	for (g = 0; g<256; g++)
	{
		LUT[g] = (int)(c*log(1+g));
	}
	// step.4-------------查表------------------------------//
	for (pCur = pGryImg; pCur<pEnd;) *(pCur++) = LUT[*pCur];
	// step.5-------------结束------------------------------//
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//逐点直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwHistogramEqualizePixel(BYTE *pGryImg, int width, int height, int N, int M, BYTE *pResImg)
{   //逐点直方图均衡化
	//该程序没有考虑优化,只是做个实验结果
	BYTE *pCur, *pEnd = pGryImg+width*height;
	BYTE *pRes;
	int x, y, sum;
	int halfN, halfM;
	int i1, i2, j1, j2, i, j;

	N = N/2*2+1;  halfN = N/2;
	M = M/2*2+1;  halfM = M/2;
	for (y = 0, pCur = pGryImg, pRes = pResImg; y<height; y++)
	{
		printf("%d\n", y);
		//计算窗口纵坐标
		i1 = y-halfN;
		if (i1<0)
		{
			i1 = 0;
			i2 = i1+N-1;
		}
		else
		{
			i2 = y+halfN;
			if (i2>height-1)
			{
				i2 = height-1;
				i1 = i2-N+1;
			}
		}
		//按行访问 
		for (x = 0; x<width; x++, pCur++, pRes++)
		{
			//计算窗口横坐标
			j1 = x-halfM;
			if (j1<0)
			{
				j1 = 0;
				j2 = j1+M-1;
			}
			else
			{
				j2 = x+halfM;
				if (j2>width-1)
				{
					j2 = width-1;
					j1 = j2-M+1;
				}
			}
			//计算名次sum
			sum = 0;
			for (i = i1; i<=i2; i++)
			{
				for (j = j1; j<=j2; j++)
				{
					if (*(pGryImg+i*width+j)<=*pCur)
					{
						sum++;
					}
				}
			}
			//赋值,按名次归一化
			*pRes = 255*sum/(N*M);
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//14Bit直方图均衡化
//
///////////////////////////////////////////////////////////////////////////////////////
int RmwRead14BitImgFile(short int *p14Img, int width, int height, char *fileName)
{
	FILE *fp;

	fp = fopen(fileName, "rb");
	if (!fp) return 1;
	fread(p14Img, sizeof(short int), width*height, fp);
	fclose(fp);
	return 0;
}

void RmwHistogramEqualize(short int *pGry14BitImg, int width, int height, BYTE *pResImg)
{
	int histogram[1<<14];
	int LUT[1<<14];
	int A, i, g;

	// step.1-------------统计直方图------------------------//
	memset(histogram, 0, sizeof(int)*(1<<14));
	for (i = 0; i<width*height; i++) histogram[*(pGry14BitImg+i)]++;
	// step.2-------------直方图均衡化----------------------//
	A = histogram[0];
	LUT[0] = 255*A/(width*height);
	for (g = 1; g<(1<<14); g++)
	{
		A += histogram[g];
		LUT[g] = 255*A/(width*height);
	}
	for (i = 0; i<width*height; i++)
	{
		pResImg[i] = LUT[*(pGry14BitImg+i)];
	}
	// step.3-------------返回-----------------------------//
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//   第一章
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
//灰度图像反相
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwInvertGryImg1(BYTE *pGryImg,int width,int height)
{
	int x, y;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pGryImg[y*width+x] = 255-pGryImg[y*width+x];
		}
	}
 	return;
}

void RmwInvertGryImg2(BYTE *pGryImg, int width, int height)
{
	int x, y;
	int pos;

	for (y = 0; y<height; y++)
	{
		pos = y*width;
		for (x = 0; x<width; x++)
		{
			pGryImg[pos+x] = 255-pGryImg[pos+x];
		}
	}
	return;
}

void RmwInvertGryImg3(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur;
	int x, y;
	int pos;

	for (y = 0; y<height; y++)
	{
		pos = y*width;
		pCur = pGryImg+pos;
		for (x = 0; x<width; x++)
		{
			pCur[x] = 255-pCur[x];
		}
	}
	return;
}

void RmwInvertGryImg4(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur;
	int x, y;

	for (y = 0,pCur=pGryImg; y<height; y++,pCur+=width)
	{
		for (x = 0; x<width; x++)
		{
			pCur[x] = 255-pCur[x];
		}
	}
	return;
}

void RmwInvertGryImg5(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur;
	int x, y;

	for (y = 0, pCur = pGryImg; y<height; y++)
	{
		for (x = 0; x<width; x++,pCur++)
		{
			*pCur = 255-*pCur;
		}
	}
	return;
}

void RmwInvertGryImg6(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur;
	int y;

	for (y = 0, pCur = pGryImg; y<height*width; y++,pCur++)
	{
		*pCur = 255-*pCur;
	}
	return;
}

void RmwInvertGryImg7(BYTE *pGryImg, int width, int height)
{
	BYTE *pCur,*pEnd=pGryImg+width*height;

	for (pCur = pGryImg; pCur<pEnd; pCur++) *pCur = 255-*pCur;
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//彩色图像反相
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwInvertRgbImg(BYTE *pRgbImg, int width, int height)
{
	BYTE *pCur, *pEnd = pRgbImg+width*height*3;

	for (pCur = pRgbImg; pCur < pEnd; pCur++) *pCur = 255-*pCur;
	return;
}
 
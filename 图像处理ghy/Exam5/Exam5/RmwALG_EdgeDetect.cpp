///////////////////////////////////////////////////////////////////////////////////////////
//
//   第四章
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
//Gradient算子
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwGradientGryImg(BYTE *pGryImg, int width, int height, BYTE *pGrdImg)
{
	BYTE *pGry, *pGrd;
	int dx, dy;
	int x, y;

	for (y = 0, pGry = pGryImg, pGrd = pGrdImg; y<height-1; y++)
	{
		for (x = 0; x<width-1; x++, pGry++)
		{
			dx = *pGry-*(pGry+1);
			dy = *pGry-*(pGry+width);
			*(pGrd++) = min(255, (int)(sqrt(dx*dx*1.0+dy*dy)));
		}
		*(pGrd++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pGrd, 0, width); //尾行不做,边缘强度赋0
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Robert算子
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwRobertsGryImg(BYTE *pGryImg, int width, int height, BYTE *pRbtImg)
{
	BYTE *pGry, *pRbt;
	int dx, dy;
	int x, y;

	for (y = 0, pGry = pGryImg, pRbt = pRbtImg; y<height-1; y++)
	{
		for (x = 0; x<width-1; x++, pGry++)
		{
			dx = *pGry-*(pGry+width+1);
			dy = *(pGry+1)-*(pGry+width);
			*(pRbt++) = max(abs(dx), abs(dy));
		}
		*(pRbt++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pRbt, 0, width); //尾行不做,边缘强度赋0
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Sobel算子
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwSobelGryImg(BYTE *pGryImg, int width, int height, BYTE *pSbImg)
{
	BYTE *pGry, *pSb;
	int dx, dy;
	int x, y;

	memset(pSbImg, 0, width); //首行不做,边缘强度赋0
	for (y = 1, pGry = pGryImg+width, pSb = pSbImg+width; y<height-1; y++)
	{
		*(pSb++) = 0;  //首列不做,边缘强度赋0
		pGry++;
		for (x = 1; x<width-1; x++,pGry++)
		{
			//求dx
			dx = *(pGry-1-width)+(*(pGry-1)*2)+*(pGry-1+width);
			dx -= *(pGry+1-width)+(*(pGry+1)*2)+*(pGry+1+width);
			//求dy
			dy = *(pGry-width-1)+(*(pGry-width)*2)+*(pGry-width+1);
			dy -= *(pGry+width-1)+(*(pGry+width)*2)+*(pGry+width+1);
			//结果
			*(pSb++) = min(255, abs(dx)+abs(dy));
		}
		*(pSb++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pSb, 0, width); //尾行不做,边缘强度赋0
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//Prewitt算子
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwPrewittGryImg(BYTE *pGryImg, int width, int height, BYTE *pPRTImg)
{
	BYTE *pGry, *pPRT;
	int dx, dy,d45,d135,v1,v2;
	int x, y;

	memset(pPRTImg, 0, width); //首行不做,边缘强度赋0
	for (y = 1, pGry = pGryImg+width, pPRT = pPRTImg+width; y<height-1; y++)
	{
		*(pPRT++) = 0;  //首列不做,边缘强度赋0
		pGry++;
		for (x = 1; x<width-1; x++, pGry++)
		{
			//求dx
			dx = *(pGry-1-width)+*(pGry-1)+*(pGry-1+width);
			dx -= *(pGry+1-width)+*(pGry+1)+*(pGry+1+width);
			//求dy
			dy = *(pGry-width-1)+*(pGry-width)+*(pGry-width+1);
			dy -= *(pGry+width-1)+*(pGry+width)+*(pGry+width+1);
			//求45度
			d45 = *(pGry-width-1)+*(pGry-width)+*(pGry-1);
			d45 -= *(pGry+width+1)+*(pGry+width)+*(pGry+1);
			//求135度
			d135 = *(pGry-width)+*(pGry-width+1)+*(pGry+1);
			d135 -= *(pGry+width-1)+*(pGry+width)+*(pGry-1);
			//结果
			v1 = max(abs(dx), abs(dy));
			v2 = max(abs(d45), abs(d135));
			*(pPRT++) = min(255, max(v1, v2));
		}
		*(pPRT++) = 0; //尾列不做,边缘强度赋0
		pGry++;
	}
	memset(pPRT, 0, width); //尾行不做,边缘强度赋0
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//沈俊算子
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwShenJunGryImg( BYTE *pGryImg, //原始灰度图像 
	                   BYTE *pTmpImg, //辅助图像
	                   int width,int height,
	                   double a0, 
	                   BYTE *pSJImg
                     )
{ 	
	BYTE *pGry,*pCur,*pSJ,*pEnd;
	int LUT[512], *ALUT; //a0查找表
	int x,y,pre,dif;

	// step.1------------初始化查找表---------------------//
	a0=min(max(0.01,a0),0.99); //安全性检查
	//a0查找表,进行了四舍五入
	ALUT=LUT+256;
	for(ALUT[0]=0,dif=1;dif<256;dif++)
	{
		ALUT[dif]=(int)(dif*a0+0.5);
		ALUT[-dif]=(int)(-dif*a0-0.5);
	}
	// step.2------------递推实现指数滤波-----------------//
	//按行滤波
	for (y = 0, pGry = pGryImg, pCur = pTmpImg; y<height; y++)
	{
		//1.从左向右: p1(y,x)=p1(y,x-1)+a*[p(y,x)-p1(y,x-1)]
		*(pCur++) = pre = *(pGry++);
		for (x = 1; x<width; x++, pGry++) *(pCur++) = pre = pre+ALUT[*pGry-pre];
		pCur--; //回到行尾
		//2.从右向左: p2(y,x)=p2(y,x+1)-a*[p1(y,x)-p2(y,x+1)]
		for (x = width-2, pCur = pCur-1; x>=0; x--) *(pCur--) = pre = pre+ALUT[*pCur-pre];
		pCur += (width+1); //回到下一行的开始
	}
	//按列滤波
	for (x = 0, pCur = pTmpImg; x<width; x++, pCur = pTmpImg+x)
	{
		//3.从上向下: p3(y,x)=p3(y-1,x)+a*[p2(y,x)-p3(y-1,x)]
		pre = *pCur;
		for (y = 1, pCur += width; y<height; y++, pCur += width) *pCur = pre = pre+ALUT[*pCur-pre];
		pCur -= width; //回到列尾
		//4.从下向上: p4(i,j)=p4(i+1,j)+a*[p3(i,j)-p4(i+1,j)]
		for (y = height-2,pCur -= width; y>=0; y--, pCur -= width) *pCur = pre = pre+ALUT[*pCur-pre];
	}
	// step.3------------正导数=1,负导数为0,0必须也是0----//
	pEnd = pTmpImg+width*height;
	for (pCur = pTmpImg, pGry = pGryImg; pCur<pEnd;pGry++)
	{
		*(pCur++) = (*pCur>*pGry);
	}
	// step.4------------过零点检测----------------------//
	memset(pSJImg, 0, width*height); //边缘强度赋0
 	pSJ = pSJImg+width; pCur = pTmpImg+width; //首行不做 
	for (y = 1; y<height-1; y++)
	{
		pSJ++; pCur++;  //首列不做
		for (x = 1; x<width-1; x++, pGry++, pCur++, pSJ++)
		{
			if (*pCur)  //正导数
			{   //下面使用4邻域,边缘为8连通,不能保证4连通;使用8邻域才能保证边缘4连通
				if ( (!*(pCur-1))|| //左,必须<=0,不能<0
					 (!*(pCur+1))|| //右,必须<=0,不能<0
					 (!*(pCur-width))|| //上,必须<=0,不能<0
					 (!*(pCur+width)) //下,必须<=0,不能<0
				   )
				{
					*pSJ = 255; //周围有导数小于等于0
				}
			}
		}
		pSJ++; pCur++;  //尾列不做
	}
	// step.5------------结束---------------------------//
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//应用示例：米粒边缘检测
//
///////////////////////////////////////////////////////////////////////////////////////////
void RmwExtractRiceEdge( BYTE *pGryImg, //原始灰度图像 
	                     BYTE *pTmpImg, //辅助图像
	                     int width,
	                     int height,
	                     double a0, 
	                     int grdThre,
	                     BYTE *pEdgeImg
                       )
{
	// step.1------------沈俊算子-----------------------//
	RmwShenJunGryImg(pGryImg, pTmpImg, width, height, a0, pEdgeImg);
	// step.2------------Sobel算子----------------------//
	RmwSobelGryImg(pGryImg, width, height, pTmpImg);
	// step.3------------二者融合-----------------------//
	for (int i = 0; i<width*height; i++)
	{
		*(pEdgeImg+i) = (pEdgeImg[i]&&(pTmpImg[i]>grdThre))*255;
	}
	// step.4------------结束---------------------------//
	return;
}

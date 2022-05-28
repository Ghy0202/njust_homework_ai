///////////////////////////////////////////////////////////////////////////////////////////
//
//第六章-轮廓填充
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
//轮廓填充算法
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwFillContour( BYTE *pGryImg, int width, int height,//待填充图像
	                 int x0, int y0, //轮廓起点
	                 bool isOuter, //是否是外轮廓
	                 BYTE *pCode, //链码
	                 int N, //链码个数
	                 int regionC, //区域内部的颜色
	                 int contourC, //轮廓点颜色
	                 int nonC //不存在的颜色
                   )
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int dADD[8]; //地址偏移量
	BYTE *pBegin, *pCur,*pTst; //轮廓起点,当前点,填充点
	int inCode, outCode, i;
	int sumC, sumDy, direction;

	// step.1-----初始化----------------------------------------//
	pBegin = pGryImg+y0*width+x0; //轮廓起点的地址
	//不同链码对应的地址偏移量
	for (i = 0; i<8; i++) dADD[i] = dy[i]*width+dx[i];
	// step.2-----轮廓点的所有链码纵坐标变化量之和赋初值----------//
	for (sumC = 2; sumC<=253; sumC++) //求sumC
	{
		if ((regionC>=sumC-2)&&(regionC<=sumC+2)) continue;
		if ((nonC>=sumC-2)&&(nonC<=sumC+2)) continue;
		break;
	}
	for(i=0,pCur=pBegin;i<N;pCur+=dADD[pCode[i]],i++) *pCur = sumC;
	// step.3-----计算轮廓点的所有链码纵坐标变化量之和------------//
	inCode = pCode[N-1]; //进入轮廓起点的链码
	for (i = 0, pCur = pBegin; i<N; i++)
	{
		outCode = pCode[i]; //从该轮廓点出发的链码
		*pCur += dy[inCode]+dy[outCode]; //像素的灰度值就是变化量之和 
		inCode = outCode; //当前轮廓点的出发码就是下一个轮廓点的进入码
		pCur += dADD[outCode]; //指向下一个轮廓点的地址
	}
	// step.4-----对填充起点和填充终点进行标记--------------------//
	for (i = 0, pCur = pBegin; i<N; pCur += dADD[pCode[i]], i++)
	{
		sumDy = *pCur;
		if ((sumDy==sumC+1)||(sumDy==sumC+2)) *pCur = regionC; //标记为填充起点
		else if ((sumDy==sumC-1)||(sumDy==sumC-2)) *pCur = nonC; //标记为填充终点
	}
	// step.5-----按行在填充起点和填充终点之间进行填充------------//
	direction = isOuter ? 1 : -1; //外轮廓是从左向右,所以是+1;内轮廓反之
	for (i = 0, pCur = pBegin; i<N; pCur += dADD[pCode[i]], i++)
	{
		if (*pCur==regionC) //找到一个填充起点
		{
			pTst = pCur;
			while (*pTst!=nonC) //一直填充到终点
			{
				*pTst = regionC;
				pTst += direction;
			}
			*pCur = nonC; //该水平段已经填充过了,避免重复填充
		}
	}
	// step.6-----对轮廓点的颜色进行赋值-------------------------//
	for(i=0,pCur=pBegin;i<N;pCur+=dADD[pCode[i]],i++) *pCur = contourC;
	// step.7-----结束-----------------------------------------//
	return;
}
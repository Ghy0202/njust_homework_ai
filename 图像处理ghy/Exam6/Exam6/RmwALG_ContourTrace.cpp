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

///////////////////////////////////////////////////////////////////////////////////////
//
//画轮廓
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwDrawContour(BYTE *pGryImg, int width, int x, int y, BYTE *pCode, int N, BYTE color)
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int i, dADD[8];
	BYTE *pCur;

	*(pGryImg+y*width+x) = color; // 为了能够处理不闭合的轮廓
	for (i = 0; i<8; i++) dADD[i] = dy[i]*width+dx[i];
	for (pCur = pGryImg+y*width+x, i = 0; i<N; i++)
	{
		pCur += dADD[pCode[i]];
		*pCur = color;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//外接矩形
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwContourRect(int x0, int y0, BYTE *pCode, int N, int *x1, int *x2, int *y1, int *y2)
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int i;

	*y1 = *y2 = y0;
	*x1 = *x2 = x0;
	for (i = 0; i<N-1; i++)
	{
		x0 = x0+dx[pCode[i]];
		y0 = y0+dy[pCode[i]];

		if (x0>*x2) *x2 = x0;
		else if (x0<*x1) *x1 = x0;

		if (y0>*y2) *y2 = y0;
		else if (y0<*y1) *y1 = y0;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//周长
//
///////////////////////////////////////////////////////////////////////////////////////
double RmwContourPerimeter(BYTE *pChainCode, int n)
{
	int no, ne, i;

	no = 0;
	for (i = 0; i<n; i++)
	{
		no += pChainCode[i]&0x01; //奇数码个数
	}
	ne = (n-no); //偶数码个数
	return ne+no*sqrt(2.0);
}
///////////////////////////////////////////////////////////////////////////////////////
//
//面积
//
///////////////////////////////////////////////////////////////////////////////////////
double RmwContourArea(BYTE *pChainCode, int n)
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int i,yi,ci;
	double S=0.0;

	yi = 0; //随意假定一个初值即可,因为面积与起点无关
	for (i = 0; i<n; i++)
	{
		ci = pChainCode[i];
		S += (yi+dy[ci]/2.0)*dx[ci];
		yi += dy[ci]; //下一个轮廓点的坐标
	}
	return fabs(S); //在轮廓逆时针走向时为正,顺时针走向时为负,所以取绝对值
}

double RmwContourPerimeterAndArea(BYTE *pChainCode, int n,double *Perimeter)
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int i, yi, ci, no;
	double S = 0.0;

	no = 0;
	yi = 0; //随意假定一个初值即可,因为面积与起点无关
	for (i = 0; i<n; i++)
	{
		ci = pChainCode[i];
		//面积计算
		S += (yi+dy[ci]/2.0)*dx[ci];
		yi += dy[ci]; //下一个轮廓点的坐标
		//周长计算
		no += ci&0x01; //奇数码个数
	}
	//周长
	*Perimeter= (n-no)+no*sqrt(2.0);
	//使用周长修正过的面积
	return fabs(S)+(*Perimeter/2);
}
///////////////////////////////////////////////////////////////////////////////////////
//
//轮廓包围的像素个数
//
///////////////////////////////////////////////////////////////////////////////////////
int RmwContourPixels(BYTE *pChainCode, int n)
{
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	static int typeLUT[8][8] = { 1,1,1,3,3,2,2,1,//0
		                         1,1,1,3,3,3,2,1,//1
		                         2,2,2,0,0,0,0,2,//2
		                         2,2,2,0,0,0,0,3,//3
		                         3,2,2,0,0,0,0,3,//4
		                         3,3,2,0,0,0,0,3,//5
		                         1,1,1,2,2,2,2,1,//6
		                         1,1,1,3,2,2,2,1 //7
	                           };
	int i, ci_1,ci,type,yi;
	int num = 0;

	num = 0;
	yi = 0; //随意假定一个初值即可,因为面积与起点无关
	ci_1 = pChainCode[n-1]; //起点的进入链码(即最末尾的链码)
	for (i = 0; i<n; i++)
	{
		ci = pChainCode[i]; //出发链码
		type = typeLUT[ci_1][ci]; //轮廓点类型
		if (type==0) num -= yi; 
		else if (type==1) num += yi+1;
		else if (type==3) num += 1;
		yi += dy[ci]; //下一个轮廓点的坐标
		ci_1 = ci; //下一个轮廓点的进入链码
	}
	return abs(num); //在轮廓逆时针走向时为正,顺时针走向时为负,所以取绝对值
}
///////////////////////////////////////////////////////////////////////////////////////
//
//像素是否被轮廓包围
//
///////////////////////////////////////////////////////////////////////////////////////
bool RmwIsPixelInContour(int x0, int y0, BYTE *pChainCode, int n, int x, int y)
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	int DY[8][8] = { 0 ,1 ,1 ,1 ,1 ,0 ,0 ,0,
		             0 ,1 ,1 ,1 ,1 ,0 ,0 ,0,
		             0 ,1 ,1 ,1 ,1 ,0 ,0 ,0,
		             0 ,1 ,1 ,1 ,1 ,0 ,0 ,0,
		            -1, 0 ,0 ,0 ,0,-1,-1,-1,
		            -1, 0 ,0 ,0 ,0,-1,-1,-1,
		            -1, 0 ,0 ,0 ,0,-1,-1,-1,
		            -1, 0 ,0 ,0 ,0,-1,-1,-1,
	              };
	int CY[8][8] = { 0, 0, 0, 0, 0, 0, 0, 0,
		             0, 0, 0, 0, 0, 1, 0, 0,
		             0, 0, 0, 0, 0, 1, 1, 0,
		             0, 0, 0, 0, 0, 1, 1, 1,
		             1, 0, 0, 0, 0, 1, 1, 1,
		             1, 1, 0, 0, 0, 1, 1, 1,
		             1, 1, 1, 0, 0, 1, 1, 1,
		             1, 1, 1, 1, 0, 1, 1, 1,
	               };
	int sum, i, V, J;
	int pre, cur;

	pre = pChainCode[n-1];
	for (sum = 0, i = 0; i<n; i++)
	{
		cur = pChainCode[i];
		V = ((x0-x)>=0) && ((y0-y)==0);
		J = ((x0-x)==0) && ((y0-y)==0);
		sum += V*DY[pre][cur]+J*CY[pre][cur];
		//Next
		x0 = x0+dx[cur];
		y0 = y0+dy[cur];
		pre=cur;
	}
	//外轮廓时,轮廓点和轮廓内包围像素的sum值都是1,轮廓外像素的sum值为0
	//内轮廓时,轮廓点和轮廓外像素的sum的值都是0,轮廓内包围像素的sum值是-1
	return (sum!=0);
}
///////////////////////////////////////////////////////////////////////////////////////
//
//图像边框清零
//
///////////////////////////////////////////////////////////////////////////////////////
void RmwSetImageBoundary(BYTE *pBinImg, int width, int height, BYTE color)
{
	BYTE *pRow;
	int y;

	memset(pBinImg, color, width);
	memset(pBinImg+(height-1)*width, color, width);
	for (y = 0, pRow = pBinImg; y<height; y++, pRow += width)
	{
		*pRow = 0;
		*(pRow+width-1) = 0;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//轮廓跟踪算法
//
///////////////////////////////////////////////////////////////////////////////////////
int RmwTraceContour( BYTE *pBinImg, int width, int height,//二值图像
	                 int x0, int y0, //轮廓起点
	                 bool isOuter, //是否是外轮廓
	                 BYTE *pChainCode, //外部申请好的一个内存空间，用来存放链码
	                 int maxChainCodeNum //可以存放的最大链码个数
                   )
{
	static int dx[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
	static int dy[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
	static int initCode[8] = { 7, 7, 1, 1, 3, 3, 5, 5 };
	int dADD[8]; //地址偏移量
	BYTE *pBegin, *pCur, *pTst; //轮廓起点,当前点,检查点
	int code, beginCode, returnCode, i;
	int N;

	// step.1-----初始化----------------------------------------//
	//不同链码对应的地址偏移量
	for(code = 0;code<8;code++) dADD[code]=dy[code]*width+dx[code];
	pBegin = pBinImg+y0*width+x0; //轮廓起点的地址
	pCur = pBegin; //当前点设置到轮廓起点
	if (isOuter) //外轮廓时的初始化
	{
		*(pCur-1) = 1;  //左侧是背景点,标记为灰度值1
		code = 7; //初始化为7
	}
	else //内轮廓时的初始化
	{
		*(pCur+1) = 1; //右侧是背景点,标记为灰度值1
		code = 3; //初始化为3
	}
	beginCode = initCode[code]; //从起点检查的第一个链码
 	// step.2-----轮廓跟踪--------------------------------------//
	N = 0; //链码个数初始化为0
	do {
		*pCur = 254; //是轮廓点,标记为灰度值254
		for (i = 0, code = initCode[code]; i<7; i++, code = (code+1)%8)
		{
			pTst = pCur+dADD[code]; //得到要检查的轮廓点的地址
			if (*pTst<2) *pTst = 1; //是背景点,标记为灰度值1
			else //是轮廓点
			{
				if (N<maxChainCodeNum) pChainCode[N++] = code; //保存链码
				if (pTst==pBegin) //回到起点的处理
				{
					//找出剩余链码的起始序号
					returnCode=(code+4)%8; //转换为于从起点出发已经检查过的链码
					for(i=0,code=beginCode;i<7;i++,code=(code+1)%8)
					{
						if (code==returnCode)
						{
							i++; //剩余链码的起始序号
							code = (code+1)%8; //剩余链码的起始值
							break;
						}
					}
					//检查剩余链码
					for (; i<7; i++, code = (code+1)%8)
					{
						pTst = pBegin+dADD[code];
						if (*pTst<2) *pTst = 1; //是背景点,标记为灰度值1
						else
						{   //保存链码
							if (N<maxChainCodeNum) pChainCode[N++] = code;
							break; //从起点开始,找到了新的轮廓点pTst
						}
					}
				}
				break; //找到了新的轮廓点pTst
			}
		}
		pCur = pTst; //当前点移动到新的轮廓点pTst
	} while (i<7); //找到轮廓点时一定有i<7
	// step.3-----结束-----------------------------------------//
	return N; //返回链码个数
}